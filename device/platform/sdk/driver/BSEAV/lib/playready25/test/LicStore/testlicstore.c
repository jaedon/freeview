/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** Most of function has additional argument using as flags to
** decide which input should be overwritten:
**   1st bit overwrite LicContext
**   2nd bit overwrite LicEnumContext (XML and XMR)
**   3rd bit overwrite g_pbLicense
**   4th bit overwrite g_pbLicense size
**   5th bit overwrite pbKID
**   6th bit overwrite pbLID
*/

#include <stdafx.h>
#include <LicStoreTestAPI.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <tclib.h>
#include <tstutils.h>
#include <tstHDSutils.h>
#include <tstXMLutils.h>
#include <drmutilities.h>
#include <drmxmlparser.h>
#include <oempkcrypto.h>
#include <drmcontextsizes.h>
#include <drmlicstore.h>
#include <tOEMIMP.h>
#include <tOEMIMP_PD.h>
#include <drmkeygenerator.h>
#include <drmwmdrmconstants.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*
** global variables
*/
static clock_t g_clockStart = 0;
static clock_t g_clockEnd   = 0;


static DRM_DST                           *g_pDatastore              = NULL;
static DRM_LICSTORE_CONTEXT               g_oContextXML             = { 0 };
static DRM_LICSTORE_CONTEXT               g_oContextXMR             = { 0 };
static DRM_LICSTORE_CONTEXT               g_oContextEMB             = { 0 };
static DRM_LICSTOREENUM_CONTEXT           g_oLicEnumContext         = { 0 };
static DRM_LICSTOREENUM_MULTIPLE_CONTEXT  g_oLicEnumContextMultiple = { 0 };
static DRM_BYTE                          *g_pbLicense               = NULL;
static DRM_KID                            g_KID                     = { 0 };
static DRM_LID                            g_LID                     = { 0 };

DRM_RESULT UpdateLicenseKeys( const DRM_WCHAR *pwszBuffer, DRM_DWORD cchBuffer, long iOption, long iNumLicense)
{
    DRM_RESULT        dr                  = DRM_SUCCESS;
    DRM_CONST_STRING  dstrLicenseBuffer   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrLIData          = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrLIDNodeData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrKIDNodeData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING  *pdstrLicenseBuffer = &dstrLicenseBuffer;
    DRM_CONST_STRING  *pdstrLIData        = &dstrLIData;
    DRM_CONST_STRING  *pdstrLIDNodeData   = &dstrLIDNodeData;
    DRM_CONST_STRING  *pdstrKIDNodeData   = &dstrKIDNodeData;
    DRM_WCHAR*        temp                = NULL;
    DRM_WCHAR         szUnique[7]         = { 0 };
    DRM_WCHAR         szNewKID[25]        = { 0 };
    DRM_DWORD         dwKIDSize           = 25;
    DRM_DWORD         dwNum               = (DRM_DWORD)iNumLicense;
    DRM_DWORD         iConvert            = NO_OF( szUnique ) - 1;
    int               i                   = 0;


    /* Check arguments */
    ChkArg( pwszBuffer != NULL );
    ChkArg( cchBuffer > g_dstrLIData.cchString );

    pdstrLicenseBuffer->pwszString = pwszBuffer;
    pdstrLicenseBuffer->cchString = cchBuffer;

    /*
    ** compose unique number
    */
    MEMSET( szUnique, 0, SIZEOF( szUnique ) );
    do
    {
        DRM_DWORD CurDigit = dwNum % 10;
        szUnique[ --iConvert ] =  ONE_WCHAR( '0' + ( DRM_BYTE )CurDigit, 0  );
        dwNum /= 10;
    }while ( iConvert > 0 );

    /* Get the licensor info data section of the license */
    ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicenseBuffer, &g_dstrLIData, NULL, NULL, NULL, pdstrLIData, g_wchForwardSlash ) );

    /*
    ** update g_LID
    */

    ChkDR( DRM_XML_GetNode( pdstrLIData, &g_dstrTagLID, NULL, NULL, 0, NULL, pdstrLIDNodeData ) );
    /* Currently, this only assigns unique values to the last five digits of the first DWORD of the LID */
    /* Since LID data is in the form of "{12345678-1234-1234-1234-123456789012}", we should skip 3 chars     */
    ChkArg( pdstrLIDNodeData->cchString == 38 );
    temp = (DRM_WCHAR*)pdstrLIDNodeData->pwszString + 3;

    for( i = 0; i < 6; i++, temp++ )
    {
        *temp = szUnique[i];
    }

    /*
    ** Update g_KID within license
    */

    ChkDR( DRM_XML_GetNode( pdstrLIData, &g_dstrTagKID, NULL, NULL, 0, NULL, pdstrKIDNodeData ) );
    ChkArg( pdstrKIDNodeData->cchString == dwKIDSize );
    temp = (DRM_WCHAR*)pdstrKIDNodeData->pwszString;
    if ( ( iOption & 0x10 ) != 0 )
    {
        ChkDR ( DRM_KG_GenerateKeyID( NULL, szNewKID, &dwKIDSize ) );
        MEMCPY( temp, szNewKID, dwKIDSize * SIZEOF( DRM_WCHAR ) );
    }

ErrorExit:

    return dr;
}


/*
**  Reinitialize all if iIndex = 0
*/
DRM_RESULT DRM_CALL TestLicStoreReinitialize( long iIndex, __in_opt char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( argv != NULL )
    {
        iIndex = strtol( argv[0], NULL, 0 );
    }

    if ( iIndex == 0 || iIndex == 1 )
    {
        ZEROMEM( &g_oContextXML, SIZEOF( g_oContextXML ) );
        ZEROMEM( &g_oContextXMR, SIZEOF( g_oContextXMR ) );
        ZEROMEM( &g_oContextEMB, SIZEOF( g_oContextEMB ) );
    }
    if ( iIndex == 0 || iIndex == 2 )
    {
        ZEROMEM( &g_oLicEnumContext, SIZEOF( g_oLicEnumContext ) );
    }
    if ( iIndex == 0 || iIndex == 3 )
    {
        SAFE_OEM_FREE( g_pbLicense );
    }
    if ( iIndex == 0 || iIndex == 4 )
    {
        ZEROMEM( &g_KID, SIZEOF( DRM_KID ) );
    }
    if ( iIndex == 0 || iIndex == 5 )
    {
        ZEROMEM( &g_LID, SIZEOF( DRM_LID ) );
    }
    else if ( iIndex == 20 )
    {
        /* Close the HDS, delete it, and reopen it */
        DRM_CONST_STRING wszHDSPath                                 = { 0 };
        DRM_WCHAR        wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};
        ChkDR( CloseHDS( g_pDatastore ) );
        ChkDR( RemoveDRMFile( RMFILE_STORE ) );
        ChkDR( tGetDeviceStorePathname( wszPathName, &wszHDSPath ) );
        ChkDR( OpenHDS( &g_pDatastore, wszPathName, FALSE ) );
    }
ErrorExit:
    return dr;
}

/*  TestLicStoreSetEmbeddedStore
**  argv[0]: Specifies the binary file to read the embedded store from.
*/
DRM_RESULT DRM_CALL TestLicStoreSetEmbeddedStore( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                        = DRM_SUCCESS;
    DRM_DST         *pDSTEmbedded              = NULL;
    DRM_EST_CONTEXT *pDSTEmbeddedContext       = NULL;
    DRM_BYTE        *pbEmbeddedStoreData       = NULL;
    DRM_BYTE        *pbEmbeddedStoreSharedData = NULL;
    DRM_DWORD        cbEmbeddedStoreData       = 0;

    ChkArg( argc    == 1 );

    MAP_TO_GLOBAL_TEST_VAR( pDSTEmbedded, DST_EMBEDDED_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pDSTEmbeddedContext, DST_EMBEDDED_CONTEXT_VAR );

    /*
    ** Attempt to close the store, but don't fail
    */
    DRM_DST_CloseStore( pDSTEmbedded );
    DRM_DST_Uninit( pDSTEmbedded );
    ZEROMEM( pDSTEmbedded, SIZEOF( DRM_DST ) );
    ZEROMEM( pDSTEmbeddedContext, SIZEOF( DRM_EST_CONTEXT ) );

    ChkDR( DRM_DST_Init ( eDRM_DST_EMBEDDED,
                          ( DRM_DST_CONTEXT * )pDSTEmbeddedContext,
                          SIZEOF( DRM_EST_CONTEXT ),
                          pDSTEmbedded ) );

    if ( argv[0] !=  NULL )
    {
        ChkBOOL( LoadTestFile( "licenses", argv[0], &pbEmbeddedStoreData, &cbEmbeddedStoreData ),
                 DRM_E_FILE_READ_ERROR );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbEmbeddedStoreSharedData, DST_EMBEDDED_STORE_DATA_VAR, cbEmbeddedStoreData );
        MEMCPY( pbEmbeddedStoreSharedData, pbEmbeddedStoreData, cbEmbeddedStoreData );

        ChkDR( DRM_DST_OpenStore( NULL, pbEmbeddedStoreSharedData, cbEmbeddedStoreData, 0, pDSTEmbedded ) );
    }
    else
    {
        /*
        ** If no file containing the embedded store is specified, just create an empty
        ** embedded store.
        */
        cbEmbeddedStoreData = MAX_EST_SIZE;
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbEmbeddedStoreSharedData, DST_EMBEDDED_STORE_DATA_VAR, cbEmbeddedStoreData );
        ZEROMEM( pbEmbeddedStoreSharedData, cbEmbeddedStoreData );

        ChkDR( DRM_DST_CreateStore( NULL,
                                    pbEmbeddedStoreSharedData,
                                    cbEmbeddedStoreData,
                                    0,
                                    eDRM_DSTBLKNUM_DWORD,
                                    TRUE,
                                    0,
                                    pDSTEmbedded ) );
    }

ErrorExit:
    SAFE_OEM_FREE( pbEmbeddedStoreData );
    return dr;
}

/*    Test API DRM_LST_Open
**  argv[0]: status of DRM_LICSTORE_CONTEXT: NULL, XML or XMR
**  argv[1]: status of DRM_DST: NULL, EMBEDDED, or Normal
**  argv[2]: optional flag to delete the store file before the API: 1 to delete
*/
DRM_RESULT DRM_CALL Test_DRM_LST_Open( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    eDRM_LICENSE_STORE_TYPE eType       = eDRM_LICENSE_STORE_XML;
    DRM_LICSTORE_CONTEXT   *pLicContext = NULL;
    DRM_DST                *pDataStore  = NULL;

    ChkArg( argc >= 2 );

    /*
    ** remove the file
    */
    if ( argc                        == 3
      && argv[2]                     != NULL
      && strtoul( argv[2], NULL, 0 ) == 1 )
    {
        TestLicStoreReinitialize( 20, NULL );
    }

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            eType = eDRM_LICENSE_STORE_XML;
            pLicContext = &g_oContextXML;

            pDataStore = g_pDatastore;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            eType = eDRM_LICENSE_STORE_XMR;
            pLicContext = &g_oContextXMR;

            pDataStore = g_pDatastore;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            eType = eDRM_LICENSE_STORE_XMR;

            MAP_TO_GLOBAL_TEST_VAR( pDataStore, DST_EMBEDDED_VAR );
            pLicContext = &g_oContextEMB;
        }
    }

    ChkDR( DRM_LST_Open( pLicContext,
                         pDataStore,
                         eType ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_Close( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr        = DRM_SUCCESS;
    DRM_LICSTORE_CONTEXT *pLicStore = NULL;

    ChkArg( argc == 2 );

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicStore = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicStore = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicStore = &g_oContextEMB;
        }
    }

    ChkDR( DRM_LST_Close( pLicStore ) );

ErrorExit:
    /*
    ** remove the file
    */
    if ( argc >= 2 && argv[1] != NULL && strtoul( argv[1], NULL, 0 ) == 1 )
    {
        TestLicStoreReinitialize( 20, NULL );
    }
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_AddLicense( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr        = DRM_SUCCESS;
    DRM_DWORD             cbLicense = 0;
    DRM_DWORD             iOption   = 0;
    DRM_LICSTORE_CONTEXT *pLicStore = NULL;

    ChkArg( argc >= 4 );

    iOption = strtoul( argv[3], NULL, 0 );

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicStore = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicStore = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicStore = &g_oContextEMB;
        }
    }

    /*
    ** the 3rd bit is for overwriting g_pbLicense
    */
    if ( ( iOption & 4 )!= 0 )
    {
        TestLicStoreReinitialize( 3, NULL );

        if ( argv[2] != NULL )
        {
            SAFE_OEM_FREE( g_pbLicense );
            /*
            ** Empty string
            */
            if ( *argv[2] == 0 )
            {
                ChkMem( g_pbLicense = ( DRM_BYTE* )Oem_MemAlloc( 1 ) );
                g_pbLicense[0] = 0;
            }
            /*
            ** load license from a file. The file name is given by argv[2]
            */
            else
            {
                ChkBOOL( LoadTestFile( "licenses", argv[2], &g_pbLicense, &cbLicense ),
                         DRM_E_FAIL );
            }
        }
    }
    /*
    ** the 4th bit is for overwriting g_pbLicense size
    */
    if ( ( iOption & 8 ) != 0 )
    {
        cbLicense = strtoul( argv[1], NULL, 0 );
    }

    ChkDR( DRM_LST_AddLicense( pLicStore, cbLicense, g_pbLicense, NULL, NULL, 0 ) );

ErrorExit:
    return dr;
}

/*
** parse given license to retrieve g_KID and g_LID
*/
DRM_RESULT DRM_CALL Test_DRM_LST_GetLicenseKeys( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_CONST_STRING wszXmlStr     = { 0 };
    DRM_CONST_STRING wszXmlSubNode = { 0 };
    DRM_CONST_STRING wszLIDValue   = { 0 };
    DRM_CONST_STRING wszKIDValue   = { 0 };
    DRM_CONST_STRING wszXmlNode    = { 0 };
    DRM_DWORD        cbKID         = DRM_ID_SIZE;
    DRM_DWORD        cbLicense     = 0;

    TestLicStoreReinitialize( 4, NULL );
    TestLicStoreReinitialize( 5, NULL );

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    /*
    ** XMR license parsing not yet implemented
    */
    ChkBOOL( strncmp( argv[0], "XML", 3 ) == 0, DRM_E_NOTIMPL );

    /*
    ** load license from a file. The file name is given by argv[1]
    */
    SAFE_OEM_FREE( g_pbLicense );
    ChkBOOL( LoadTestFile( "licenses", argv[1], &g_pbLicense, &cbLicense ), DRM_E_FAIL );

    wszXmlStr.pwszString = ( const DRM_WCHAR* )g_pbLicense;
    wszXmlStr.cchString = cbLicense >> 1;

    /*
    ** extract the version string
    */
    ChkDR( DRM_XML_GetNode( &wszXmlStr,
                            &g_dstrLicense,
                             NULL,
                             NULL,
                             0,
                            &wszXmlNode,
                            &wszXmlSubNode ) );

    /*
    ** extract LicensorInfo node
    */
    wszXmlStr = wszXmlSubNode;
    ChkDR( DRM_XML_GetNode( &wszXmlStr,
                            &g_dstrLicensorInfoNode,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &wszXmlSubNode ) );

    /*
    ** extract Data node
    */
    wszXmlStr = wszXmlSubNode;
    ChkDR( DRM_XML_GetNode( &wszXmlStr, &g_dstrTagData, NULL, NULL, 0,NULL, &wszXmlSubNode ) );

    /*
    ** extract g_LID, g_KID and priority node
    */
    wszXmlStr = wszXmlSubNode;
    ChkDR( DRM_XML_GetNode( &wszXmlStr, &g_dstrTagLID, NULL, NULL, 0,&wszXmlNode, &wszLIDValue ) );
    ChkDR( DRM_XML_GetNode( &wszXmlStr, &g_dstrTagKID, NULL, NULL, 0,&wszXmlNode, &wszKIDValue ) );

    /*
    ** convert extracted g_LID to GUID
    */
    ChkDR( DRM_UTL_StringToLittleEndianGuid( &wszLIDValue, ( DRM_GUID* )g_LID.rgb ) );

    /*
    ** base64 decode g_KID
    */
    ChkDR( DRM_B64_DecodeW( &wszKIDValue, (DRM_DWORD *)&cbKID, g_KID.rgb, 0 ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_GetLicense( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr           = DRM_SUCCESS;
    DRM_DWORD               cbLicense    = 0;
    DRM_DWORD               iOption      = 0;
    DRM_DWORD               iLIDNullFlag = 0;
    DRM_DWORD               iKIDNullFlag = 0;
    DRM_LICSTORE_CONTEXT   *pLicContext  = NULL;

    ChkArg( argc >= 6 );

    iOption = strtoul( argv[5], NULL, 0 );

    /*
    ** 3rd bit is for overwriting g_pbLicense
    */
    if ( ( iOption & 4 ) != 0 )
    {
        TestLicStoreReinitialize( 3, NULL );

        if ( argv[3] != NULL )
        {
            SAFE_OEM_FREE( g_pbLicense );
            ChkMem( g_pbLicense = ( DRM_BYTE* )Oem_MemAlloc( ( DRM_DWORD ) DRMCRT_strlen( argv[3] ) + 1 ) );
            MEMCPY( g_pbLicense, argv[3], DRMCRT_strlen( argv[3] ) + 1 );
        }
    }
    /*
    ** 4th bit is for overwriting g_pbLicense size
    */
    if ( ( iOption & 8 ) != 0 )
    {
        cbLicense = strtoul( argv[4], NULL, 0 );
    }
    /*
    ** 5th bit for overwriting g_KID
    */
    if ( ( iOption & 16 ) != 0 )
    {
        TestLicStoreReinitialize( 4, NULL );

        if ( argv[1] != NULL )
        {
            DRM_SIZE_T cb = DRMCRT_strlen( argv[1] );
            MEMCPY( g_KID.rgb, argv[1], min( cb + 1, DRM_ID_SIZE ) );
        }
        else
        {
            iKIDNullFlag = 1;
        }
    }
    /*
    ** 6th bit is for overwriting g_LID
    */
    if ( ( iOption & 32 ) != 0 )
    {
        TestLicStoreReinitialize( 5, NULL );

        if ( argv[2] != NULL )
        {
            DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
            MEMCPY( g_LID.rgb, argv[2], min( cb + 1, DRM_ID_SIZE ) );
        }
        else
        {
            iLIDNullFlag = 1;
        }
    }

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicContext = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicContext = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicContext = &g_oContextEMB;
        }
    }

    dr = DRM_LST_GetLicense( pLicContext,
                             iKIDNullFlag > 0 ? NULL : &g_KID,
                             iLIDNullFlag > 0 ? NULL : &g_LID,
                             NULL,
                             g_pbLicense,
                             ( iOption & 1 ) != 0 ? NULL : &cbLicense );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        SAFE_OEM_FREE( g_pbLicense );
        ChkMem( g_pbLicense = (DRM_BYTE *)Oem_MemAlloc( cbLicense + 1 ) );
    }
    else
    {
        ChkDR( dr );
    }
    ChkDR( DRM_LST_GetLicense( pLicContext,
                              &g_KID,
                              &g_LID,
                               NULL,
                               g_pbLicense,
                               ( iOption&1 ) ? NULL : &cbLicense ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_InitEnum( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr                   = DRM_SUCCESS;
    DRM_DWORD               iEnumContextNullFlag = 0;
    DRM_DWORD               iKIDNullFlag         = 0;
    DRM_BOOL                fPrioritized         = FALSE;
    DRM_DWORD               iOption              = 0;
    DRM_LICSTORE_CONTEXT   *pLicContext          = NULL;
    DRM_CONST_STRING        wszKIDValue          = { 0 };
    DRM_DWORD               cbKID                = SIZEOF( DRM_KID );
    DRM_WCHAR               pwszAttr[256]        = { 0 };

    ChkArg( argc >= 5 );

    iOption = strtoul( argv[4], NULL, 0 );

    /*
    ** g_KID
    */
    if ( ( iOption & 16 ) != 0 )
    {
        TestLicStoreReinitialize( 4, NULL );
        if ( argv[1] != NULL )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
            TST_UTL_mbstowcs( pwszAttr, argv[1], cch );
            wszKIDValue.pwszString = pwszAttr;
            wszKIDValue.cchString = cch;

            /*
            ** base64 decode KID
            */
            ZEROMEM( &g_KID, SIZEOF( DRM_KID ) );
            ChkDR( DRM_B64_DecodeW( &wszKIDValue, &cbKID, g_KID.rgb, 0 ) );
        }
        else
        {
            iKIDNullFlag = 1;
        }
    }
    /*
    ** fPrioritized flag
    */
    if ( strtoul( argv[2], NULL, 0 ) == 1 )
    {
        fPrioritized = TRUE;
    }

    /*
    ** enum context
    */
    if ( argv[3] != NULL )
    {
        MEMSET( &g_oLicEnumContext, 0, SIZEOF( g_oLicEnumContext ) );
        MEMCPY( &g_oLicEnumContext, argv[3], DRMCRT_strlen( argv[3] ) + 1 );
    }
    else
    {
        iEnumContextNullFlag = 1;
    }

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicContext = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicContext = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicContext = &g_oContextEMB;
        }
    }

    ChkDR( DRM_LST_InitEnum( pLicContext,
                             iKIDNullFlag>0 ?NULL:&g_KID,
                             fPrioritized,iEnumContextNullFlag>0 ? NULL : &g_oLicEnumContext ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_InitReverseEnum( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr                   = DRM_SUCCESS;
    DRM_DWORD               iEnumContextNullFlag = 0;
    DRM_DWORD               iKIDNullFlag         = 0;
    DRM_DWORD               iOption              = 0;
    DRM_LICSTORE_CONTEXT   *pLicContext          = NULL;
    DRM_CONST_STRING        wszKIDValue          = { 0 };
    DRM_DWORD               cbKID                = SIZEOF( DRM_KID );
    DRM_WCHAR               pwszAttr[256]        = { 0 };

    ChkArg( argc >= 4 );

    iOption = strtoul( argv[3], NULL, 0 );

    /*
    ** g_KID
    */
    if ( ( iOption & 16 ) != 0 )
    {
        TestLicStoreReinitialize( 4, NULL );
        if ( argv[1] != NULL )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
            TST_UTL_mbstowcs( pwszAttr, argv[1], cch );
            wszKIDValue.pwszString = pwszAttr;
            wszKIDValue.cchString = cch;

            /*
            ** base64 decode KID
            */
            ZEROMEM( &g_KID, SIZEOF( DRM_KID ) );
            ChkDR( DRM_B64_DecodeW( &wszKIDValue, &cbKID, g_KID.rgb, 0 ) );
        }
        else
        {
            iKIDNullFlag = 1;
        }
    }

    /*
    ** enum context
    */
    if ( argv[2] != NULL )
    {
        MEMSET( &g_oLicEnumContext, 0, SIZEOF( g_oLicEnumContext ) );
        MEMCPY( &g_oLicEnumContext, argv[2], DRMCRT_strlen( argv[2] ) + 1 );
    }
    else
    {
        iEnumContextNullFlag = 1;
    }

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicContext = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicContext = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicContext = &g_oContextEMB;
        }
    }

    ChkDR( DRM_LST_InitReverseEnum( pLicContext,
                                    iKIDNullFlag>0 ?NULL:&g_KID,
                                    iEnumContextNullFlag>0 ? NULL : &g_oLicEnumContext ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_EnumNext( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_DWORD        iKIDNullFlag  = 0;
    DRM_DWORD        iLIDNullFlag  = 0;
    DRM_DWORD        iSizeNullFlag = 0;
    DRM_KID          pVerifyKID    = { 0 };
    DRM_LID          pVerifyLID    = { 0 };
    DRM_DWORD        cbLicense     = 0;
    DRM_CONST_STRING wszLIDValue   = { 0 };
    DRM_CONST_STRING wszKIDValue   = { 0 };
    DRM_DWORD        cbKID         = 0;
    DRM_WCHAR        pwszAttr[256] = { 0 };

    ChkArg( argc >= 6 );

    /*
    ** get g_KID and g_LID
    */
    if ( argv[1] != NULL )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[1] );
        MEMCPY( g_KID.rgb, argv[1], min( cb + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iKIDNullFlag = 1;
    }

    if ( argv[2] != NULL )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        MEMCPY( g_LID.rgb, argv[2], min( cb + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iLIDNullFlag = 1;
    }

    /*
    ** license size
    */
    if ( argv[3] != NULL )
    {
        cbLicense = strtoul( argv[3], NULL, 0 );
    }
    else
    {
        iSizeNullFlag = 1;
    }
    ChkDR( DRM_LST_EnumNext( argv[0] != NULL ? &g_oLicEnumContext : NULL,
                             iKIDNullFlag > 0 ? NULL : &g_KID,
                             iLIDNullFlag > 0 ? NULL : &g_LID,
                             NULL,
                             iSizeNullFlag > 0 ? NULL : &cbLicense ) );

    /*
    ** verify the results
    */
    if ( argv[4] != NULL && iKIDNullFlag != 1 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs( pwszAttr, argv[4], cch );
        wszKIDValue.pwszString = pwszAttr;
        wszKIDValue.cchString = cch;

        /*
        ** base64 decode g_KID
        */
        ZEROMEM( &pVerifyKID, SIZEOF( DRM_KID ) );

        cbKID = SIZEOF( DRM_KID );

        ChkDR( DRM_B64_DecodeW( &wszKIDValue, &cbKID, pVerifyKID.rgb, 0 ) );

        ChkBOOL( strncmp( (const DRM_CHAR *)pVerifyKID.rgb, (const DRM_CHAR *)g_KID.rgb, DRM_ID_SIZE ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( argv[5] != NULL && iLIDNullFlag != 1 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[5] ), &cch ) );
        ZEROMEM( pwszAttr, SIZEOF( pwszAttr ) );
        TST_UTL_mbstowcs( pwszAttr, argv[5], cch + 1 );
        wszLIDValue.pwszString = pwszAttr;
        wszLIDValue.cchString= cch;
        ZEROMEM( &pVerifyLID, SIZEOF( DRM_LID ) );
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &wszLIDValue, ( DRM_GUID* )pVerifyLID.rgb ) );

        ChkBOOL( strncmp( (const DRM_CHAR *)pVerifyLID.rgb, (const DRM_CHAR *)g_LID.rgb,DRM_ID_SIZE ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    /*
    ** Validate license size against expected
    */
    if ( argc > 6 && argv[6] != NULL && iSizeNullFlag == 0 )
    {
        ChkBOOL( cbLicense == strtoul( argv[6], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_EnumDelete( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( DRM_LST_EnumDelete( argv[0] != NULL ? &g_oLicEnumContext : NULL ) );
PREFAST_POP

ErrorExit:
    return dr;
}

/*
** Test_DRM_LST_InitEnumMultiple
** argv[0] : Specifies the store(s) to use (NONE, XML, XMR, BOTH, or NULL).
** argv[1] : Specifies the KID to use (may be NULL). Option flag 16 must be set to read this.
** argv[2] : fPrioritized flag (TRUE or FALSE). Defaults to FALSE.
** argv[3] : Enumeration context (NULL or from command line).
** argv[4] : Option flags: 16 specifies that the KID is read from argv[1]
*/
DRM_RESULT DRM_CALL Test_DRM_LST_InitEnumMultiple( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr                   = DRM_SUCCESS;
    DRM_DWORD               iEnumContextNullFlag = 0;
    DRM_DWORD               iKIDNullFlag         = 0;
    DRM_BOOL                fPrioritized         = FALSE;
    DRM_DWORD               iOption              = 0;
    DRM_DWORD               cLicContexts         = 0;
    DRM_LICSTORE_CONTEXT   *pLicContexts[DRM_LICSTORE_MAX_STORES] = { 0 };
    DRM_CONST_STRING        wszKIDValue          = { 0 };
    DRM_DWORD               cbKID                = 0;
    DRM_WCHAR               pwszAttr[256]        = { 0 };

    ChkArg( argc >= 5 );

    iOption = strtoul( argv[4], NULL, 0 );

    /*
    ** g_KID
    */
    if ( ( iOption & 16 ) != 0 )
    {
        TestLicStoreReinitialize( 4, NULL );
        if ( argv[1] != NULL )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
            TST_UTL_mbstowcs( pwszAttr, argv[1], cch );
            wszKIDValue.pwszString = pwszAttr;
            wszKIDValue.cchString  = cch;
            cbKID = SIZEOF( DRM_KID );
            ChkDR( DRM_B64_DecodeW( &wszKIDValue, &cbKID, g_KID.rgb, 0 ) );
        }
        else
        {
            iKIDNullFlag = 1;
        }
    }
    /*
    ** fPrioritized flag
    */
    if ( strtoul( argv[2], NULL, 0 ) == 1 )
    {
        fPrioritized = TRUE;
    }

    /*
    ** enum context
    */
    if ( argv[3] != NULL )
    {
        MEMSET( &g_oLicEnumContextMultiple, 0, SIZEOF( g_oLicEnumContextMultiple ) );
        MEMCPY( &g_oLicEnumContextMultiple, argv[3], DRMCRT_strlen( argv[3] ) + 1 );
    }
    else
    {
        iEnumContextNullFlag = 1;
    }

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicContexts[0] = &g_oContextXML;
            cLicContexts    = 1;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicContexts[0] = &g_oContextXMR;
            cLicContexts    = 1;
        }
        else if ( strncmp( argv[0], "BOTH1", 5 ) == 0 )
        {
            pLicContexts[0] = &g_oContextXML;
            pLicContexts[1] = &g_oContextXMR;
            cLicContexts    = 2;
        }
        else if ( strncmp( argv[0], "BOTH2", 5 ) == 0 )
        {
            pLicContexts[0] = &g_oContextXMR;
            pLicContexts[1] = &g_oContextEMB;
            cLicContexts    = 2;
        }
        else if ( strncmp( argv[0], "BOTH3", 5 ) == 0 )
        {
            pLicContexts[0] = &g_oContextXML;
            pLicContexts[1] = &g_oContextEMB;
            cLicContexts    = 2;
        }
        else
        {
            ChkArg( strncmp( argv[0], "NONE", 4 ) == 0 );
        }
    }

    ChkDR( DRM_LST_InitEnumMultiple( pLicContexts,
                                     cLicContexts,
                                     iKIDNullFlag>0 ?NULL:&g_KID,
                                     fPrioritized,iEnumContextNullFlag>0 ? NULL : &g_oLicEnumContextMultiple ) );
ErrorExit:
    return dr;
}

/*
** Test_DRM_LST_EnumNextMultiple
** argv[0] : Specifies the enumeration context to use (NULL or normal)
** argv[1] : Specifies the KID parameter (NULL or from input)
** argv[2] : Specifies the LID parameter (NULL or from input)
** argv[3] : Specifies the size parameter (NULL or from input)
** argv[4] : Specifies the expected KID, or NULL if the KID is not verified.
** argv[5] : Specifies the expected LID, or NULL if the LID is not verified.
** argv[6] : (optional) Specifies the expected license store for the next license (XML or XMR).
*/
DRM_RESULT DRM_CALL Test_DRM_LST_EnumNextMultiple( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_DWORD        iKIDNullFlag   = 0;
    DRM_DWORD        iLIDNullFlag   = 0;
    DRM_DWORD        iSizeNullFlag  = 0;
    DRM_KID          pVerifyKID     = { 0 };
    DRM_LID          pVerifyLID     = { 0 };
    DRM_DWORD        cbLicense      = 0;
    DRM_CONST_STRING wszLIDValue    = { 0 };
    DRM_CONST_STRING wszKIDValue    = { 0 };
    DRM_DWORD        cbKID          = 0;
    DRM_WCHAR        pwszAttr[256]  = { 0 };
    DRM_LICSTORE_CONTEXT *pLicStore = NULL;

    ChkArg( argc >= 6 );

    /*
    ** get g_KID and g_LID
    */
    if ( argv[1] != NULL )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[1] );
        MEMCPY( g_KID.rgb, argv[1], min( cb + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iKIDNullFlag = 1;
    }

    if ( argv[2] != NULL )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        MEMCPY( g_LID.rgb, argv[2], min( cb + 1, DRM_ID_SIZE ) );
    }
    else
    {
        iLIDNullFlag = 1;
    }

    /*
    ** license size
    */
    if ( argv[3] != NULL )
    {
        cbLicense = strtoul( argv[3], NULL, 0 );
    }
    else
    {
        iSizeNullFlag = 1;
    }
    ChkDR( DRM_LST_EnumNextMultiple( argv[0] != NULL ? &g_oLicEnumContextMultiple : NULL,
                                     iKIDNullFlag > 0 ? NULL : &g_KID,
                                     iLIDNullFlag > 0 ? NULL : &g_LID,
                                     &pLicStore,
                                     NULL,
                                     iSizeNullFlag > 0 ? NULL : &cbLicense ) );

    /*
    ** verify the results
    */
    if ( argv[4] != NULL && iKIDNullFlag != 1 )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs( pwszAttr, argv[4], cch );
        wszKIDValue.pwszString = pwszAttr;
        wszKIDValue.cchString = cch;

        /*
        ** base64 decode g_KID
        */
        ZEROMEM( &pVerifyKID, SIZEOF( DRM_KID ) );

        cbKID = SIZEOF( DRM_KID );

        ChkDR( DRM_B64_DecodeW( &wszKIDValue, &cbKID, pVerifyKID.rgb, 0 ) );

        ChkBOOL( strncmp( (const DRM_CHAR *)pVerifyKID.rgb, (const DRM_CHAR *)g_KID.rgb, DRM_ID_SIZE ) == 0, DRM_E_FAIL );
    }
    if ( argv[5] != NULL && iLIDNullFlag != 1 )
    {
        DRM_DWORD cch = 0;
        ZEROMEM( pwszAttr, SIZEOF( pwszAttr ) );
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[5] ), &cch ) );
        TST_UTL_mbstowcs( pwszAttr, argv[5], cch + 1 );
        wszLIDValue.pwszString = pwszAttr;
        wszLIDValue.cchString= cch;
        ZEROMEM( &pVerifyLID, SIZEOF( DRM_LID ) );
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &wszLIDValue, ( DRM_GUID* )pVerifyLID.rgb ) );

        ChkBOOL( strncmp( (const DRM_CHAR *)pVerifyLID.rgb, (const DRM_CHAR *)g_LID.rgb,DRM_ID_SIZE ) == 0, DRM_E_FAIL );
    }
    if ( argc > 6 && argv[6] != NULL )
    {
        if ( strncmp( "XML", argv[6], 3 ) == 0 )
        {
            ChkBOOL( pLicStore == &g_oContextXML, DRM_E_FAIL );
        }
        else if ( strncmp( "XMR", argv[6], 3 ) == 0 )
        {
            ChkBOOL( pLicStore == &g_oContextXMR, DRM_E_FAIL );
        }
        else
        {
            ChkArg( strncmp( "EMB", argv[6], 3 ) == 0 );
            ChkBOOL( pLicStore == &g_oContextEMB, DRM_E_FAIL );
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_LST_EnumDeleteMultiple( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_LST_EnumDeleteMultiple( argv[0] != NULL ? &g_oLicEnumContextMultiple : NULL ) );

ErrorExit:
    return dr;
}

/*
** Performance test
*/
DRM_RESULT DRM_CALL TestLicStoreAddMultiLicenses( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr          = DRM_SUCCESS;
    DRM_DWORD             cbLicense   = 0;
    DRM_DWORD             iOption     = 0;
    DRM_DWORD             iNumLicense = 0;
    DRM_DWORD             iNumKeyBase = 0;
    DRM_LICSTORE_CONTEXT *pLicContext = NULL;

    ChkArg( argc    >= 5 );
    ChkArg( argv[1] != NULL );

    iOption = strtoul( argv[4], NULL, 0 );
    iNumLicense = strtoul( argv[2], NULL, 0 );
    iNumKeyBase = strtoul( argv[3], NULL, 0 );

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicContext = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicContext = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicContext = &g_oContextEMB;
        }
    }

    Log( "Trace", "\t\tStart adding %d licenses to the store.", iNumLicense );

    /*
    ** load license from a file. The file name is given by argv[1]
    */
    SAFE_OEM_FREE( g_pbLicense );
    ChkBOOL( LoadTestFile( "licenses", argv[1], &g_pbLicense, &cbLicense ), DRM_E_FAIL );

    while( iNumLicense-- > 0 )
    {
        UpdateLicenseKeys( ( const DRM_WCHAR* )g_pbLicense, cbLicense / SIZEOF( DRM_WCHAR ), (long)iOption, (long)iNumKeyBase );

        ChkDR( DRM_LST_AddLicense( pLicContext, cbLicense, g_pbLicense, NULL, NULL, 0 ) );
        iNumKeyBase++;
    }
    Log( "Trace", "\t\tSuccessfully added the licenses to the store." );

ErrorExit:
    return dr;
}

/*at least four arguments.
** 1st is the index to indicate
**      0: remove old file;
**      1: start an action;
**      2: end an action;
**      3: log a statement;
**      4: remove the log file.
** 2nd is the description of the action.
*/
DRM_RESULT DRM_CALL TestLicStoreLog( long argc, __in_ecount(argc) char **argv )
{
#ifdef WINCE_TEST
    return DRM_SUCCESS;
#else
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD  iActionType    = 0;
    FILE      *fp             = NULL;
    DRM_CHAR  *szFileName     = NULL;
    DRM_DWORD  cchFileName    = 0;

    ChkArg( argc >= 2 );

    ChkDR( TST_UTL_MakePathA( NULL, "LicstorePerfLog.txt", &szFileName, &cchFileName, 0 ) );

    iActionType = strtoul( argv[0], NULL, 0 );

    switch ( iActionType )
    {
    case 0:
        Log( "Trace", "\t\t****** %s ******", argv[1] );
        fp = fopen( szFileName, "w+" );
        break;
    case 1:
        g_clockStart = clock( );
        break;
    case 2:
        g_clockEnd = clock( );
        Log( "Trace", "%s. It took %ld milliseconds.", argv[1], g_clockEnd - g_clockStart );
        fp = fopen( szFileName, "a+" );
        break;
    case 3:
        Log( "Trace", "%s.", argv[1] );
        fp = fopen( szFileName, "a+" );
        break;
    case 4:
        ChkDR( tRemoveFile( szFileName ) );
        break;
    default:
        ChkArg( FALSE );
    }

    if ( iActionType !=  1 && iActionType != 4 )
    {
        ChkBOOL( fp != NULL, DRM_E_FAIL );
        fclose( fp );
    }
ErrorExit:
    SAFE_OEM_FREE( szFileName );
    return dr;
#endif
}

DRM_RESULT DRM_CALL Test_DRM_LST_DeleteLicense( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr        = DRM_SUCCESS;
    DRM_LICSTORE_CONTEXT *pLicStore = NULL;
    DRM_KID               oKID      = { 0 };
    DRM_LID               oLID      = { 0 };
    DRM_CONST_STRING      dstrKID   = EMPTY_DRM_STRING;
    DRM_CONST_STRING      dstrLID   = EMPTY_DRM_STRING;
    DRM_DWORD             cbSize    = 0;

    ChkArg( argc >= 3 );

    if ( argv[ 0 ] != NULL )
    {
        if ( strncmp( argv[0], "XML", 3 ) == 0 )
        {
            pLicStore = &g_oContextXML;
        }
        else if ( strncmp( argv[0], "XMR", 3 ) == 0 )
        {
            pLicStore = &g_oContextXMR;
        }
        else
        {
            ChkArg( strncmp( argv[0], "EMB", 3 ) == 0 );
            pLicStore = &g_oContextEMB;
        }
    }

    ChkDR( MakeDRMString( &dstrKID, argv[ 1 ] ) );

    ChkDRMString( &dstrKID );

    cbSize = SIZEOF( DRM_KID );

    ChkDR( DRM_B64_DecodeW( &dstrKID,
                            &cbSize,
                            ( DRM_BYTE* )&oKID,
                            0 ) );

    ChkDR( MakeDRMString( &dstrLID, argv[ 2 ] ) );

    ChkDRMString( &dstrLID );

    ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrLID, ( DRM_GUID * )&oLID ) );

    ChkDR( DRM_LST_DeleteLicense( pLicStore, &oKID, &oLID, NULL ) );

ErrorExit:
    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrLID );
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(LicStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT       dr                  = DRM_SUCCESS;
    DRM_CONST_STRING wszHDSPath          = { 0 };
    DRM_DST         *pDSTEmbedded        = NULL;
    DRM_EST_CONTEXT *pDSTEmbeddedContext = NULL;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    RemoveDRMFile( RMFILE_STORE );

    ChkDR( tGetDeviceStorePathname( wszPathName, &wszHDSPath ) );
    ChkDR( OpenHDS( &g_pDatastore, wszHDSPath.pwszString, FALSE ) );

    /*
    ** Initialize shared variables
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pDSTEmbedded, DST_EMBEDDED_VAR, SIZEOF( DRM_DST ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pDSTEmbeddedContext, DST_EMBEDDED_CONTEXT_VAR, SIZEOF( DRM_EST_CONTEXT ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(LicStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_LST_Close( &g_oContextXML );
    DRM_LST_Close( &g_oContextXMR );
    DRM_LST_Close( &g_oContextEMB );

    ChkDR( CloseHDS( g_pDatastore ) );
    g_pDatastore = NULL;
    SAFE_OEM_FREE( g_pbLicense );
ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(LicStore)
    API_ENTRY( Test_DRM_LST_Open )
    API_ENTRY( Test_DRM_LST_Close )
    API_ENTRY( Test_DRM_LST_AddLicense )
    API_ENTRY( Test_DRM_LST_GetLicense )
    API_ENTRY( Test_DRM_LST_GetLicenseKeys )
    API_ENTRY( Test_DRM_LST_EnumDelete )
    API_ENTRY( TestLicStoreReinitialize )
    API_ENTRY( TestLicStoreAddMultiLicenses )
    API_ENTRY( Test_DRM_LST_InitEnum )
    API_ENTRY( Test_DRM_LST_InitReverseEnum )
    API_ENTRY( Test_DRM_LST_EnumNext )
    API_ENTRY( TestLicStoreLog )
    API_ENTRY( Test_DRM_LST_DeleteLicense )
    API_ENTRY( Test_DRM_LST_InitEnumMultiple )
    API_ENTRY( Test_DRM_LST_EnumNextMultiple )
    API_ENTRY( Test_DRM_LST_EnumDeleteMultiple )
    API_ENTRY( TestLicStoreSetEmbeddedStore )
END_TEST_API_LIST

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PKTEST_NAMESPACE_CODE
