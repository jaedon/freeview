/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmdatastore.h>
#include <DatastoreTestAPI.h>
#include <drmmathsafe.h>
#include <drmhdsblockheadercache.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define DST_TEST_STORE_MAX 1000 /* Max size of the store data buffer */

static const DRM_DST_KEY g_oDefaultKey1 = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
static const DRM_DST_KEY g_oDefaultKey2 = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

DRMINLINE DRM_SIZE_T localmin( DRM_SIZE_T a, DRM_SIZE_T b ) { return min(a,b); }

/********************************************************************************************
** Function:    TestDatastoreInit
**
** Synopsis:    Initializes the Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_eImplementationType (from DRM_DST_IMPLEMENTATION_TYPE or INVALID)
**              argv[1] : f_pDSTContext (Optional. normal or NULL)
**              argv[2] : f_cbDSTContext (Optional. Defaults to SIZEOF( DRM_DST_CONTEXT ))
**              argv[3] : f_pDST (Optional. normal or NULL)
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreInit( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DST         *pDST        = NULL;
    DRM_DST_CONTEXT *poDSTContext = NULL;
    DRM_DWORD        dwImplType   = eDRM_DST_NONE;
    DRM_DWORD        cbDSTContext = DRM_DST_CONTEXT_LEN;

    ChkArg( argc    >= 1 );
    ChkArg( argv[0] != NULL );

    /*
    ** Set dwImplType
    */
    if ( strncmp( argv[0], "eDRM_DST_NONE", 13 ) != 0 )
    {
        if ( strncmp( argv[0], "INVALID", 7 ) == 0 )
        {
            dwImplType = 100; /* Must be out of range for DRM_DST_IMPLEMENTATION_TYPE */
        }
        else if ( strncmp( argv[0], "eDRM_DST_HDS", 12 ) == 0 )
        {
            dwImplType = eDRM_DST_HDS;
        }
        else if ( strncmp( argv[0], "eDRM_DST_EMBEDDED", 17 ) == 0 )
        {
            dwImplType = eDRM_DST_EMBEDDED;
        }
        else
        {
            ChkArg( FALSE );
        }
    }

    /*
    ** Set up DST Context
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( poDSTContext, DST_CONTEXT_VAR );
    }

    /*
    ** Set up cbDSTContext
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        cbDSTContext = strtoul( argv[2], NULL, 0 );
    }

    /*
    ** Set up DST
    */
    if ( argc < 4 || argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    ChkDR( DRM_DST_Init( ( DRM_DST_IMPLEMENTATION_TYPE )dwImplType, poDSTContext, cbDSTContext, pDST ) );

ErrorExit:
     return dr;
}


/********************************************************************************************
** Function:    TestDatastoreUninit
**
** Synopsis:    Uninitializes the Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreUninit( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DST    *pDST = NULL;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    ChkDR( DRM_DST_Uninit( pDST ) );

ErrorExit:
     return dr;
}

static DRM_RESULT _MakeStoreName( DRM_BYTE *pbDest, DRM_DWORD *pcchDest, const DRM_CHAR *pszSource )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_CONST_STRING    dstrTemp    = { 0 };

    ChkArg( pcchDest  != NULL );
    ChkArg( pszSource != NULL );

    ChkDR( TST_UTL_MakePathString( NULL, pszSource, &dstrTemp ) );

    ChkBOOL( dstrTemp.cchString * SIZEOF(DRM_WCHAR) < *pcchDest, DRM_E_BUFFERTOOSMALL );

    ChkArg( pbDest != NULL );

    MEMCPY( pbDest, dstrTemp.pwszString, dstrTemp.cchString * SIZEOF(DRM_WCHAR) );

    *pcchDest = dstrTemp.cchString;

ErrorExit:
    SAFE_OEM_FREE( dstrTemp.pwszString );
    return dr;
}

/**********************************************************************
** Function:    TestDatastoreCreateStore
**
** Synopsis:    Creates a data store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (optional. NORMAL or NULL)
**              argv[1] : f_pbStoreData (optional. NULL, NORMAL, or specifies a name. Defaults to empty buffer)
**              argv[2] : f_cbStoreData (optional. If NULL or not specified, use actual buffer size)
**              argv[3] : f_dwBlockSize (optional. Defaults to 0)
**              argv[4] : f_eBlNumType (optional. Defaults to eDRM_DSTBLKNUM_DWORD. From DRM_DST_BLKNUMTYPE or INVALID)
**              argv[5] : f_fTruncateExist (optional. Defaults to TRUE)
**              argv[6] : f_dwInitSizeInKB (optional. Defaults to 0)
**
***********************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCreateStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_DST    *pDST           = NULL;
    DRM_BYTE   *pbStoreData    = NULL;
    DRM_DWORD  *pcbStoreData   = NULL;
    DRM_DWORD   cbStoreData    = 0;
    DRM_DWORD   dwBlockSize    = 0;
    DRM_DWORD   dwBlNumType    = eDRM_DSTBLKNUM_DWORD;
    DRM_BOOL    fTruncate      = TRUE;
    DRM_DWORD   dwInitSize     = 0;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up pbStoreData (and default cbStoreData)
    */
    MAP_TO_GLOBAL_TEST_VAR( pcbStoreData, DST_PCBSTOREDATA_VAR );
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pbStoreData, DST_PBSTOREDATA_VAR );

        if ( argc > 1 && strncmp( argv[1], "NORMAL", 6 ) != 0 )
        {
            *pcbStoreData = ( DRM_DWORD )strlen( argv[1] );
            ChkBOOL( *pcbStoreData < DST_TEST_STORE_MAX, DRM_E_BUFFERTOOSMALL );

            /*
            ** Store data is null-terminated string
            */
            if ( pDST != NULL && pDST->eImplementationType == eDRM_DST_HDS ) /* Upgrade to WCHAR for HDS name */
            {
                *pcbStoreData = DST_TEST_STORE_MAX;
                ChkDR( _MakeStoreName( pbStoreData, pcbStoreData, argv[1] ) );
            }
            else
            {
                MEMCPY( pbStoreData, ( DRM_BYTE * )argv[1], *pcbStoreData );
            }
        }
        else
        {
            ZEROMEM( pbStoreData, *pcbStoreData );
        }
    }

    /*
    ** Set up cbStoreData
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        cbStoreData = strtoul( argv[2], NULL, 0 );
        pcbStoreData = &cbStoreData;
    }

    /*
    ** Set up f_dwBlockSize
    */
    if ( argc > 3 && argv[3] != NULL )
    {
        dwBlockSize = strtoul( argv[3], NULL, 0 );
    }

    /*
    ** Set up f_eBlNumType
    */
    if ( argc > 4 && argv[4] != NULL )
    {
        if ( strncmp( argv[4], "INVALID", 7 ) == 0 )
        {
            dwBlNumType = 100;  /* Must be out of range for DRM_DST_BLKNUMTYPE */
        }
        else if ( strncmp( argv[4], "eDRM_DSTBLKNUM_WORD", 19 ) == 0 )
        {
            dwBlNumType = eDRM_DSTBLKNUM_WORD;
        }
        else if ( strncmp( argv[4], "eDRM_DSTBLKNUM_DWORD", 20 ) != 0 )
        {
            ChkArg( FALSE );
        }
    }

    /*
    ** Set up f_fTruncateExists
    */
    if ( argc > 5 && argv[5] != NULL )
    {
        if ( strncmp( argv[5], "TRUE", 4 ) == 0 )
        {
            fTruncate = TRUE;
        }
        else if ( strncmp( argv[5], "FALSE", 5 ) != 0 )
        {
            ChkArg( FALSE );
        }
    }

    /*
    ** Set up f_dwInitSizeInKB
    */
    if ( argc > 6 && argv[6] != NULL )
    {
        dwInitSize = strtoul( argv[6], NULL, 0 );
    }

    ChkDR( DRM_DST_CreateStore( NULL,
                                pbStoreData,
                                *pcbStoreData,
                                dwBlockSize,
                                ( DRM_DST_BLKNUMTYPE )dwBlNumType,
                                fTruncate,
                                dwInitSize,
                                pDST ) );

ErrorExit:
     return dr;
}

/**********************************************************************
** Function:    TestDatastoreOpenStore
**
** Synopsis:    Opens a data store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (optional. NORMAL or NULL)
**              argv[1] : f_pbStoreData (optional. NULL, NORMAL, or specifies a name)
**              argv[2] : f_cbStoreData (optional. If NULL or not specified, use actual buffer size)
**              argv[3] : f_dwGrowBySizeinKB (optional. Defaults to 0)
**
***********************************************************************/
DRM_RESULT DRM_CALL TestDatastoreOpenStore  ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_DST    *pDST           = NULL;
    DRM_BYTE   *pbStoreData    = NULL;
    DRM_DWORD   cbStoreData    = 0;
    DRM_DWORD  *pcbStoreData   = NULL;
    DRM_DWORD   dwGrowSize     = 0;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up pbStoreData (and default cbStoreData)
    */
    MAP_TO_GLOBAL_TEST_VAR( pcbStoreData, DST_PCBSTOREDATA_VAR );
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pbStoreData, DST_PBSTOREDATA_VAR );

        if ( argc > 1 && strncmp( argv[1], "NORMAL", 6 ) != 0 )
        {
            *pcbStoreData = ( DRM_DWORD )strlen( argv[1] );
            ChkBOOL( *pcbStoreData < DST_TEST_STORE_MAX, DRM_E_BUFFERTOOSMALL );

            /*
            ** Store data is null-terminated string
            */
            if ( pDST != NULL && pDST->eImplementationType == eDRM_DST_HDS ) /* Upgrade to WCHAR for HDS name */
            {
                *pcbStoreData = DST_TEST_STORE_MAX;
                ChkDR( _MakeStoreName( pbStoreData, pcbStoreData, argv[1] ) );
            }
            else
            {
                MEMCPY( pbStoreData, ( DRM_BYTE * )argv[1], *pcbStoreData );
            }
        }
    }

    /*
    ** Set up cbStoreData
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        cbStoreData = strtoul( argv[2], NULL, 0 );
        pcbStoreData = &cbStoreData;
    }

    /*
    ** Set up f_dwGrowBySizeinKB
    */
    if ( argc > 3 && argv[3] != NULL )
    {
        dwGrowSize = strtoul( argv[3], NULL, 0 );
    }

    ChkDR( DRM_DST_OpenStore( NULL,
                              pbStoreData,
                              *pcbStoreData,
                              dwGrowSize,
                              pDST ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCloseStore
**
** Synopsis:    Closes the Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCloseStore ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DST    *pDST = NULL;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    ChkDR( DRM_DST_CloseStore( pDST ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCommitStore
**
** Synopsis:    Commits the Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCommitStore ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DST    *pDST = NULL;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    ChkDR( DRM_DST_CommitStore( pDST ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCleanupStore
**
** Synopsis:    Performs cleanup on the Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_fWait (TRUE or FALSE) - Optional. Defaults to TRUE.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCleanupStore ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DST    *pDST = NULL;
    DRM_BOOL    fWait = TRUE;

    /*
    ** Set up DST
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up fWait
    */
    if ( argc > 1 && argv[1] != NULL )
    {
        if ( strncmp( argv[1], "TRUE", 4 ) != 0 )
        {
            ChkArg( strncmp( argv[1], "FALSE", 5 ) == 0 );
            fWait = FALSE;
        }
    }

    ChkDR( DRM_DST_CleanupStore( pDST, fWait ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreOpenNamespace
**
** Synopsis:    Opens a namespace in a Data Store using the given or default parameters.
**
** Arguments:   argv[0] : f_pNamespace (NULL, or copy from arg string)
**              argv[1] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_eOpenMode - Optional (from DRM_DST_OPENMODE or INVALID). Defaults to eDRM_DST_OPEN_ALWAYS.
**              argv[3] : f_wMaxNumChildren - Optional. Defaults to 10.
**              argv[4] : f_dwExpectedSlotSize - Optional. Defaults to 100.
**              argv[5] : f_pContextNDS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[6] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreOpenNamespace ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr                 = DRM_SUCCESS;
    DRM_DST                   *pDST               = NULL;
    const DRM_DST_NAMESPACE   *poNS               = NULL;
    DRM_DST_NAMESPACE          oNS                = { 0 };
    DRM_DST_NAMESPACE_CONTEXT *pNsContexts        = NULL;
    DRM_LONG                   iNsContextNull     = 0;
    DRM_LONG                   iNamespaceIndex    = 0;
    DRM_WORD                   wMaxNumChildren    = 10;
    DRM_DWORD                  dwOpenMode         = eDRM_DST_OPEN_ALWAYS;
    DRM_DWORD                  dwExpectedSlotSize = 100;

    ChkArg( argc > 0 );

    /*
    ** Set up poNS
    */
    if ( argv[0] != NULL )
    {
        MEMCPY( &oNS, argv[0], localmin( ( DRM_DWORD )strlen( argv[0] ), SIZEOF( DRM_DST_NAMESPACE ) ) );
        poNS = &oNS;
    }

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set dwOpenMode
    */
    if ( argc > 2
      && argv[2] != NULL
      && strncmp( argv[2], "eDRM_DST_OPEN_ALWAYS", 20 ) != 0 )
    {
        if ( strncmp( argv[2], "INVALID", 7 ) == 0 )
        {
            dwOpenMode = 100; /* Must be out of range for DRM_DST_OPENMODE */
        }
        else if ( strncmp( argv[2], "eDRM_DST_CREATE_NEW", 19 ) == 0 )
        {
            dwOpenMode = eDRM_DST_CREATE_NEW;
        }
        else if ( strncmp( argv[2], "eDRM_DST_OPEN_EXISTING", 22 ) == 0 )
        {
            dwOpenMode = eDRM_DST_OPEN_EXISTING;
        }
        else
        {
            ChkArg( FALSE );
        }
    }

    /*
    ** Set up wMaxNumChildren
    */
    if ( argc > 3 && argv[3] != NULL )
    {
        wMaxNumChildren = ( DRM_WORD )strlen( argv[3] );
    }

    /*
    ** Set up dwExpectedSlotSize
    */
    if ( argc > 4 && argv[4] != NULL )
    {
        dwExpectedSlotSize = ( DRM_DWORD )strlen( argv[4] );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    MAP_TO_GLOBAL_TEST_VAR( pNsContexts, DST_NAMESPACE_CONTEXT_VAR );

    if ( argc > 6 )
    {
        iNamespaceIndex=(int)atoi( argv[6] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    if ( argc > 5 && argv[5] == NULL )
    {
        iNsContextNull = 1;
    }

    ChkDR( DRM_DST_OpenNamespace(   pDST,
                                    poNS,
                ( DRM_DST_OPENMODE )dwOpenMode,
                                    wMaxNumChildren,
                                    dwExpectedSlotSize,
                                    iNsContextNull > 0 ? NULL : &pNsContexts[iNamespaceIndex] ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCloseNamespace
**
** Synopsis:    Closes the Namespace using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextNS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCloseNamespace ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr               = DRM_SUCCESS;
    DRM_DST                   *pDST             = NULL;
    DRM_DST_NAMESPACE_CONTEXT *pNSContexts       = NULL;
    DRM_LONG                   iNamespaceIndex  = 0;
    DRM_LONG                   iNsContextNull     = 0;

    MAP_TO_GLOBAL_TEST_VAR( pNSContexts, DST_NAMESPACE_CONTEXT_VAR );

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    if ( argc > 1 && argv[1] == NULL )
    {
        iNsContextNull = 1;
    }

    if ( argc > 2 )
    {
        iNamespaceIndex=(int)atoi( argv[2] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    ChkDR( DRM_DST_CloseNamespace(  pDST,
                                    iNsContextNull > 0 ? NULL : &( pNSContexts[iNamespaceIndex] ) ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreDeleteNamespace
**
** Synopsis:    Deletes the Namespace using the given or default parameters.
**
** Arguments:   argv[0] : f_pNamespace (NULL, or copy from arg string)
**              argv[1] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_fWait (TRUE or FALSE) - Optional. Defaults to TRUE.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreDeleteNamespace ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT         dr    = DRM_SUCCESS;
    DRM_DST           *pDST = NULL;
    DRM_DST_NAMESPACE *poNS  = NULL;
    DRM_DST_NAMESPACE  oNS   = { 0 };
    DRM_BOOL           fWait = TRUE;

    ChkArg( argc > 0 );

    /*
    ** Set up poNS
    */
    if ( argv[0] != NULL )
    {
        MEMCPY( &oNS, argv[0], localmin( ( DRM_DWORD )strlen( argv[0] ), SIZEOF( DRM_DST_NAMESPACE ) ) );
        poNS = &oNS;
    }

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up fWait
    */
    if ( argc > 1 && argv[1] != NULL )
    {
        if ( strncmp( argv[1], "TRUE", 4 ) != 0 )
        {
            ChkArg( strncmp( argv[1], "FALSE", 5 ) == 0 );
            fWait = FALSE;
        }
    }

    ChkDR( DRM_DST_DeleteNamespace( pDST, poNS, fWait ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCommitNamespace
**
** Synopsis:    Commits the Namespace using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextNS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCommitNamespace ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr         = DRM_SUCCESS;
    DRM_DST                   *pDST       = NULL;
    DRM_DST_NAMESPACE_CONTEXT *pNSContexts       = NULL;
    DRM_LONG                   iNamespaceIndex  = 0;
    DRM_LONG                   iNsContextNull     = 0;

    MAP_TO_GLOBAL_TEST_VAR( pNSContexts, DST_NAMESPACE_CONTEXT_VAR );

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */

    if ( argc > 1 && argv[1] == NULL )
    {
        iNsContextNull = 1;
    }

    if ( argc > 2 )
    {
        iNamespaceIndex=(int)atoi( argv[2] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    ChkDR( DRM_DST_CommitNamespace( pDST, iNsContextNull > 0 ? NULL : &( pNSContexts[iNamespaceIndex] ) ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreMakeSlotHint
**
** Synopsis:    Creates a slot hint using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pSlotHint (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreMakeSlotHint ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_DST              *pDST         = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext = NULL;
    DRM_DST_SLOT_HINT    *pSlotHint    = NULL;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_HINT
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotHint, DST_SLOT_HINT_VAR );
    }

    ChkDR( DRM_DST_MakeSlotHint( pDST, pSlotContext, pSlotHint ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreOpenSlot
**
** Synopsis:    Opens a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextNS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_dwMode (DWORD from argv[2]) - Optional. Defaults to eDRM_DST_OPEN_ALWAYS & eDRM_DST_LOCKEXCLUSIVE
**              argv[3] : f_poKey1 (NULL, DEFAULT, or from input) - Optional.
**              argv[4] : f_poKey2 (NULL, DEFAULT, or from input) - Optional.
**              argv[5] : f_pSlotHint (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[6] : f_pcbSlotSize (NULL, DEFAULT, or from input) - Optional. Defaults to 100.
**              argv[7] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[8] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreOpenSlot ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr                 = DRM_SUCCESS;
    DRM_DST                   *pDST               = NULL;
    DRM_DST_SLOT_CONTEXT      *pSlotContext       = NULL;
    DRM_DST_SLOT_HINT         *pSlotHint          = NULL;
    DRM_DST_NAMESPACE_CONTEXT *pNsContexts        = NULL;
    DRM_LONG                   iNsContextNull     = 0;
    DRM_LONG                   iNamespaceIndex    = 0;
    DRM_DWORD                  dwMode             = eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKEXCLUSIVE;
    DRM_DST_KEY                oKey1              = { 0 };
    DRM_DST_KEY                oKey2              = { 0 };
    DRM_DST_KEY               *pKey1              = ( DRM_DST_KEY * )&g_oDefaultKey1;
    DRM_DST_KEY               *pKey2              = ( DRM_DST_KEY * )&g_oDefaultKey2;
    DRM_DWORD                  cbSlotSize         = 100;
    DRM_DWORD                  cbSlotSizeExpected = 0;
    DRM_DWORD                 *pcbSlotSize        = &cbSlotSize;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    MAP_TO_GLOBAL_TEST_VAR( pNsContexts, DST_NAMESPACE_CONTEXT_VAR );

    if ( argc > 8 )
    {
        iNamespaceIndex=(int)atoi( argv[8] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    if ( argc > 1 && argv[1] == NULL )
    {
        iNsContextNull = 1;
    }

    /*
    ** Set up dwMode
    */
    if ( argc > 2 && argv[2] != NULL )
    {
        dwMode = ( DRM_DWORD )strtoul( argv[2], NULL, 0 );
    }

    /*
    ** Set up pKey1
    */
    if ( argc > 3 )
    {
        if ( argv[3] == NULL )
        {
            pKey1 = NULL;
        }
        else if ( strncmp( argv[3], "DEFAULT", 7 ) != 0 )
        {
            pKey1 = &oKey1;
            MEMCPY( pKey1, argv[3], localmin( ( DRM_DWORD )strlen( argv[3] ), SIZEOF( DRM_DST_KEY ) ) );
        }
    }

    /*
    ** Set up pKey2
    */
    if ( argc > 4 )
    {
        if ( argv[4] == NULL )
        {
            pKey2 = NULL;
        }
        else if ( strncmp( argv[4], "DEFAULT", 7 ) != 0 )
        {
            pKey2 = &oKey2;
            MEMCPY( pKey2, argv[4], localmin( ( DRM_DWORD )strlen( argv[4] ), SIZEOF( DRM_DST_KEY ) ) );
        }
    }

    /*
    ** Set up DRM_DST_SLOT_HINT
    */
    if ( argc < 6 || argv[5] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotHint, DST_SLOT_HINT_VAR );
    }

    /*
    ** Set up f_pcbSlotSize
    */
    if ( argc > 6 )
    {
        if ( argv[6] == NULL )
        {
            pcbSlotSize = NULL;
        }
        else if ( strncmp( argv[6], "DEFAULT", 7 ) != 0 )
        {
            cbSlotSize = ( DRM_DWORD )strtoul( argv[6], NULL, 0 );
        }
    }
    cbSlotSizeExpected = cbSlotSize;

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 8 || argv[7] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    ChkDR( DRM_DST_OpenSlot( pDST,
                             iNsContextNull > 0 ? NULL : &pNsContexts[iNamespaceIndex],
                             dwMode,
                             pKey1,
                             pKey2,
                             pSlotHint,
                             pcbSlotSize,
                             pSlotContext ) );

    /*
    ** Verify output parameter
    */
    if ( pcbSlotSize != NULL )
    {
        ChkBOOL( *pcbSlotSize == cbSlotSizeExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreCloseSlot
**
** Synopsis:    Closes the Slot using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCloseSlot ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_DST              *pDST         = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext = NULL;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    ChkDR( DRM_DST_CloseSlot( pDST, pSlotContext ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreDeleteSlot
**
** Synopsis:    Deletes a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextNS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_poKey1 (NULL, DEFAULT, or from input) - Optional. Defaults to all-zero key.
**              argv[3] : f_poKey2 (NULL, DEFAULT, or from input) - Optional. Defaults to all-zero key.
**              argv[4] : f_pSlotHint (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[5] : f_fWait (TRUE or FALSE) - Optional. Defaults to TRUE.
**              argv[6] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreDeleteSlot ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr           = DRM_SUCCESS;
    DRM_DST                   *pDST         = NULL;
    DRM_DST_SLOT_HINT         *pSlotHint    = NULL;
    DRM_DST_NAMESPACE_CONTEXT *pNsContexts        = NULL;
    DRM_LONG                   iNsContextNull     = 0;
    DRM_LONG                   iNamespaceIndex    = 0;
    DRM_DST_KEY                oKey1        = { 0 };
    DRM_DST_KEY                oKey2        = { 0 };
    DRM_DST_KEY               *pKey1        = ( DRM_DST_KEY * )&g_oDefaultKey1;
    DRM_DST_KEY               *pKey2        = ( DRM_DST_KEY * )&g_oDefaultKey2;
    DRM_BOOL                   fWait        = TRUE;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    MAP_TO_GLOBAL_TEST_VAR( pNsContexts, DST_NAMESPACE_CONTEXT_VAR );

    if ( argc > 6 )
    {
        iNamespaceIndex=(int)atoi( argv[6] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    if ( argc > 1 && argv[1] == NULL )
    {
        iNsContextNull = 1;
    }

    /*
    ** Set up pKey1
    */
    if ( argc > 2 )
    {
        if ( argv[2] == NULL )
        {
            pKey1 = NULL;
        }
        else if ( strncmp( argv[2], "DEFAULT", 7 ) != 0 )
        {
            pKey1 = &oKey1;
            MEMCPY( pKey1, argv[2], localmin( ( DRM_DWORD )strlen( argv[2] ), SIZEOF( DRM_DST_KEY ) ) );
        }
    }

    /*
    ** Set up pKey2
    */
    if ( argc > 3 )
    {
        if ( argv[3] == NULL )
        {
            pKey2 = NULL;
        }
        else if ( strncmp( argv[3], "DEFAULT", 7 ) != 0 )
        {
            pKey2 = &oKey2;
            MEMCPY( pKey2, argv[3], localmin( ( DRM_DWORD )strlen( argv[3] ), SIZEOF( DRM_DST_KEY ) ) );
        }
    }

    /*
    ** Set up DRM_DST_SLOT_HINT
    */
    if ( argc < 5 || argv[4] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotHint, DST_SLOT_HINT_VAR );
    }

    /*
    ** Set up fWait
    */
    if ( argc > 5 && argv[5] != NULL )
    {
        if ( strncmp( argv[5], "TRUE", 4 ) != 0 )
        {
            ChkArg( strncmp( argv[5], "FALSE", 5 ) == 0 );
            fWait = FALSE;
        }
    }

    ChkDR( DRM_DST_DeleteSlot( pDST,
                               iNsContextNull > 0 ? NULL : &pNsContexts[iNamespaceIndex],
                               pKey1,
                               pKey2,
                               pSlotHint,
                               fWait ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreInitSlotEnum
**
** Synopsis:    Initialized a slot enumeration using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextNS (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_poKey1 (NULL, DEFAULT, or from input) - Optional.
**              argv[3] : f_eLockMode - Optional (from DRM_DST_LOCKMODE or INVALID). Defaults to eDRM_DST_LOCKEXCLUSIVE.
**              argv[4] : f_pContextEnum (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[5] : namespace index
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreInitSlotEnum ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr           = DRM_SUCCESS;
    DRM_DST                   *pDST         = NULL;
    DRM_DST_NAMESPACE_CONTEXT *pNsContexts        = NULL;
    DRM_LONG                   iNsContextNull     = 0;
    DRM_LONG                   iNamespaceIndex    = 0;
    DRM_DST_KEY                oKey1        = { 0 };
    DRM_DST_KEY               *pKey1        = ( DRM_DST_KEY * )&g_oDefaultKey1;
    DRM_DWORD                  dwLockMode   = eDRM_DST_LOCKEXCLUSIVE;
    DRM_DST_ENUM_CONTEXT      *pContextEnum = NULL;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DST_NAMESPACE_CONTEXT
    */
    MAP_TO_GLOBAL_TEST_VAR( pNsContexts, DST_NAMESPACE_CONTEXT_VAR );

    if ( argc > 5 )
    {
        iNamespaceIndex=(int)atoi( argv[5] );
    }

    if( iNamespaceIndex >= MAX_NAMESPACE_NUM || iNamespaceIndex < 0 )
    {
        iNamespaceIndex = 0;
    }

    if ( argc > 1 && argv[1] == NULL )
    {
        iNsContextNull = 1;
    }

    /*
    ** Set up pKey1
    */
    if ( argc > 2 )
    {
        if ( argv[2] == NULL )
        {
            pKey1 = NULL;
        }
        else if ( strncmp( argv[2], "DEFAULT", 7 ) != 0 )
        {
            pKey1 = &oKey1;
            MEMCPY( pKey1, argv[2], localmin( ( DRM_DWORD )strlen( argv[2] ), SIZEOF( DRM_DST_KEY ) ) );
        }
    }

    /*
    ** Set dwLockMode
    */
    if ( argc > 3 && argv[3] != NULL && strncmp( argv[3], "eDRM_DST_LOCKEXCLUSIVE", 22 ) != 0 )
    {
        if ( strncmp( argv[3], "eDRM_DST_LOCKSHARED", 18 ) == 0 )
        {
            dwLockMode = eDRM_DST_LOCKSHARED;
        }
        else if ( strncmp( argv[3], "eDRM_DST_LOCKWAIT", 17 ) == 0 )
        {
            dwLockMode = eDRM_DST_LOCKWAIT;
        }
        else if ( strncmp( argv[3], "INVALID", 7 ) == 0 )
        {
            dwLockMode = 100; /* Must be out of range for DRM_DST_LOCKMODE */
        }
    }

    /*
    ** Set up f_pContextEnum
    */
    if ( argc < 5 || argv[4] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pContextEnum, DST_ENUM_CONTEXT_VAR );
    }

    ChkDR( DRM_DST_InitSlotEnum( pDST,
                                 iNsContextNull > 0 ? NULL : &pNsContexts[iNamespaceIndex],
                                 pKey1,
                                 ( DRM_DST_LOCKMODE )dwLockMode,
                                 pContextEnum ) );

ErrorExit:
     return dr;
}


/********************************************************************************************
** Function:    TestDatastoreSlotEnumNext
**
** Synopsis:    Retrieves the next element from a slot enumeration using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextEnum (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[3] : f_pKey1 (NULL, DEFAULT, or from input) - Optional.
**              argv[4] : f_pKey2 (NULL, DEFAULT, or from input) - Optional.
**              argv[5] : f_pcbSize (NULL, DEFAULT, or from input) - Optional. Defaults to 100.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotEnumNext ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT              dr             = DRM_SUCCESS;
    DRM_DST                *pDST           = NULL;
    DRM_DST_ENUM_CONTEXT   *pContextEnum   = NULL;
    DRM_DST_SLOT_CONTEXT   *pSlotContext   = NULL;
    DRM_DST_KEY             oKey1          = { 0 };
    DRM_DST_KEY             oKey2          = { 0 };
    DRM_DST_KEY             oKey1Expected  = { 0 };
    DRM_DST_KEY             oKey2Expected  = { 0 };
    DRM_DST_KEY            *pKey1          = &oKey1;
    DRM_DST_KEY            *pKey2          = &oKey2;
    DRM_DWORD               cbSize         = 0;
    DRM_DWORD              *pcbSize        = &cbSize;
    DRM_DWORD               cbSizeExpected = 100;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up f_pContextEnum
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pContextEnum, DST_ENUM_CONTEXT_VAR );
    }

    /*
    ** Set up f_pContextSlot
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    /*
    ** Set up pKey1
    */
    if ( argc > 3 && argv[3] == NULL )
    {
        pKey1 = NULL;
    }
    else if ( argc < 4 || strncmp( argv[3], "DEFAULT", 7 ) == 0 )
    {
        MEMCPY( &oKey1Expected, &g_oDefaultKey1, SIZEOF( DRM_DST_KEY ) );
    }
    else
    {
        MEMCPY( &oKey1Expected, argv[3], localmin( ( DRM_DWORD )strlen( argv[3] ), SIZEOF( DRM_DST_KEY ) ) );
    }

    /*
    ** Set up pKey2
    */
    if ( argc > 4 && argv[4] == NULL )
    {
        pKey2 = NULL;
    }
    else if ( argc < 5 || strncmp( argv[4], "DEFAULT", 7 ) == 0 )
    {
        MEMCPY( &oKey2Expected, &g_oDefaultKey2, SIZEOF( DRM_DST_KEY ) );
    }
    else
    {
        MEMCPY( &oKey2Expected, argv[4], localmin( ( DRM_DWORD )strlen( argv[4] ), SIZEOF( DRM_DST_KEY ) ) );
    }

    /*
    ** Set up pcbSize
    */
    if ( argc > 5 )
    {
        if ( argv[5] == NULL )
        {
            pcbSize = NULL;
        }
        else if ( strncmp( argv[5], "DEFAULT", 7 ) != 0 )
        {
            cbSizeExpected = ( DRM_DWORD )strtoul( argv[5], NULL, 0 );
        }
    }

    ChkDR( DRM_DST_SlotEnumNext( pDST,
                                 pContextEnum,
                                 pSlotContext,
                                 pKey1,
                                 pKey2,
                                 pcbSize ) );

    /*
    ** Compare OUT parameters to expected values
    */
    if ( pKey1 != NULL )
    {
        ChkBOOL( MEMCMP( pKey1, &oKey1Expected, SIZEOF( DRM_DST_KEY ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( pKey2 != NULL )
    {
        ChkBOOL( MEMCMP( pKey2, &oKey2Expected, SIZEOF( DRM_DST_KEY ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( pcbSize != NULL )
    {
        ChkBOOL( *pcbSize == cbSizeExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreSlotEnumReloadCurrent
**
** Synopsis:    Reloads the current element from a slot enumeration using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextEnum (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[3] : f_pKey1 (NULL, DEFAULT, or from input) - Optional.
**              argv[4] : f_pKey2 (NULL, DEFAULT, or from input) - Optional.
**              argv[5] : f_pcbSize (NULL, DEFAULT, or from input) - Optional. Defaults to 100.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotEnumReloadCurrent ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT              dr             = DRM_SUCCESS;
    DRM_DST                *pDST           = NULL;
    DRM_DST_ENUM_CONTEXT   *pContextEnum   = NULL;
    DRM_DST_SLOT_CONTEXT   *pSlotContext   = NULL;
    DRM_DST_KEY             oKey1          = { 0 };
    DRM_DST_KEY             oKey2          = { 0 };
    DRM_DST_KEY             oKey1Expected  = { 0 };
    DRM_DST_KEY             oKey2Expected  = { 0 };
    DRM_DST_KEY            *pKey1          = &oKey1;
    DRM_DST_KEY            *pKey2          = &oKey2;
    DRM_DWORD               cbSize         = 0;
    DRM_DWORD              *pcbSize        = &cbSize;
    DRM_DWORD               cbSizeExpected = 100;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up f_pContextEnum
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pContextEnum, DST_ENUM_CONTEXT_VAR );
    }

    /*
    ** Set up f_pContextSlot
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    /*
    ** Set up pKey1
    */
    if ( argc > 3 && argv[3] == NULL )
    {
        pKey1 = NULL;
    }
    else if ( argc < 4 || strncmp( argv[3], "DEFAULT", 7 ) == 0 )
    {
        MEMCPY( &oKey1Expected, &g_oDefaultKey1, SIZEOF( DRM_DST_KEY ) );
    }
    else
    {
        MEMCPY( &oKey1Expected, argv[3], localmin( ( DRM_DWORD )strlen( argv[3] ), SIZEOF( DRM_DST_KEY ) ) );
    }

    /*
    ** Set up pKey2
    */
    if ( argc > 4 && argv[4] == NULL )
    {
        pKey2 = NULL;
    }
    else if ( argc < 5 || strncmp( argv[4], "DEFAULT", 7 ) == 0 )
    {
        MEMCPY( &oKey2Expected, &g_oDefaultKey2, SIZEOF( DRM_DST_KEY ) );
    }
    else
    {
        MEMCPY( &oKey2Expected, argv[4], localmin( ( DRM_DWORD )strlen( argv[4] ), SIZEOF( DRM_DST_KEY ) ) );
    }

    /*
    ** Set up pcbSize
    */
    if ( argc > 5 )
    {
        if ( argv[5] == NULL )
        {
            pcbSize = NULL;
        }
        else if ( strncmp( argv[5], "DEFAULT", 7 ) != 0 )
        {
            cbSizeExpected = ( DRM_DWORD )strtoul( argv[5], NULL, 0 );
        }
    }

    ChkDR( DRM_DST_SlotEnumReloadCurrent( pDST,
                                          pContextEnum,
                                          pSlotContext,
                                          pKey1,
                                          pKey2,
                                          pcbSize ) );

    /*
    ** Compare OUT parameters to expected values
    */
    if ( pKey1 != NULL )
    {
        ChkBOOL( MEMCMP( pKey1, &oKey1Expected, SIZEOF( DRM_DST_KEY ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( pKey2 != NULL )
    {
        ChkBOOL( MEMCMP( pKey2, &oKey2Expected, SIZEOF( DRM_DST_KEY ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( pcbSize != NULL )
    {
        ChkBOOL( *pcbSize == cbSizeExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
     return dr;
}


/********************************************************************************************
** Function:    TestDatastoreSlotEnumDeleteCurrent
**
** Synopsis:    Reloads the current element from a slot enumeration using the given or default parameters.
**
** Arguments:   argv[0] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[1] : f_pContextEnum (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotEnumDeleteCurrent ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT              dr             = DRM_SUCCESS;
    DRM_DST                *pDST           = NULL;
    DRM_DST_ENUM_CONTEXT   *pContextEnum   = NULL;
    DRM_DST_SLOT_CONTEXT   *pSlotContext   = NULL;

    /*
    ** Set up DST
    */
    if ( argc < 1 || argv[0] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up f_pContextEnum
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pContextEnum, DST_ENUM_CONTEXT_VAR );
    }

    /*
    ** Set up f_pContextSlot
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    ChkDR( DRM_DST_SlotEnumDeleteCurrent( pDST,
                                          pContextEnum,
                                          pSlotContext ) );


ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreSlotResize
**
** Synopsis:    Resized a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_cbSize
**              argv[1] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotResize ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr            = DRM_SUCCESS;
    DRM_DST              *pDST          = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext  = NULL;
    DRM_DWORD             cbSize        = 0;

    ChkArg( argc > 0 && argv[0] != NULL );

    /*
    ** Set up f_cbSize
    */
    cbSize = ( DRM_DWORD )strtoul( argv[0], NULL, 0 );

    /*
    ** Set up DST
    */
    if ( argc < 2 || argv[1] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    ChkDR( DRM_DST_SlotResize( pDST, pSlotContext, cbSize ) );

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreSlotSeek
**
** Synopsis:    Seek within a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_lOffset
**              argv[1] : f_eSeekMode - Optional (from DRM_DST_SEEKMODE or INVALID). Defaults to eDRM_DST_SEEKCUR.
**              argv[1] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[2] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[3] : f_pdwSeekPointer - Optional (expected value). Defaults to NULL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotSeek ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr                   = DRM_SUCCESS;
    DRM_DST              *pDST                 = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext         = NULL;
    DRM_LONG             lOffset               = 0;
    DRM_DWORD            dwSeekMode            = eDRM_DST_SEEKCUR;
    DRM_DWORD           *pdwSeekPointer        = NULL;
    DRM_DWORD            dwSeekPointer         = 0;
    DRM_DWORD            dwSeekPointerExpected = 0;

    ChkArg( argc > 0 && argv[0] != NULL );

    /*
    ** Set up f_lOffset
    */
    lOffset = ( DRM_LONG )strtol( argv[0], NULL, 0 );

    /*
    ** Set up f_eSeekMode
    */
    if ( argc > 1 && argv[1] != NULL && strncmp( argv[1], "eDRM_DST_SEEKCUR", 16 ) != 0 )
    {
        if ( strncmp( argv[1], "INVALID", 7 ) == 0 )
        {
            dwSeekMode = 100;  /* Must be out of range for DRM_DST_SEEKMODE */
        }
        else if ( strncmp( argv[1], "eDRM_DST_SEEKEND", 16 ) == 0 )
        {
            dwSeekMode = eDRM_DST_SEEKEND;
        }
        else
        {
            ChkArg( strncmp( argv[1], "eDRM_DST_SEEKSET", 16 ) == 0 )
            dwSeekMode = eDRM_DST_SEEKSET;
        }
    }

    /*
    ** Set up DST
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 4 || argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }

    /*
    ** Set up f_pdwSeekPointer
    */
    if ( argc == 5 && argv[4] != NULL )
    {
        dwSeekPointerExpected = ( DRM_DWORD )strtoul( argv[4], NULL, 0 );
        pdwSeekPointer = &dwSeekPointer;
    }

    ChkDR( DRM_DST_SlotSeek( pDST,
                             pSlotContext,
                             lOffset,
                             ( DRM_DST_SEEKMODE )dwSeekMode,
                             pdwSeekPointer ) );

    /*
    ** Compare with expected value
    */
    if ( pdwSeekPointer != NULL )
    {
        ChkBOOL( *pdwSeekPointer == dwSeekPointerExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
     return dr;
}

/********************************************************************************************
** Function:    TestDatastoreSlotRead
**
** Synopsis:    Read from a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_pbData (NULL, DEFAULT, or from input) - Optional. Expected value of pbData - Defaults to all 0, 10 bytes.
**              argv[1] : f_cbData - Optional. Defaults to full length of f_pbData.
**              argv[2] : f_pcbRead - Optional. Defaults to f_cbData, otherwise specifies NULL or expected bytes read.
**              argv[3] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[4] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotRead ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_DST              *pDST             = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext     = NULL;
    DRM_DWORD             cbData           = 0;
    DRM_BYTE             *pbData           = NULL;
    DRM_BYTE             *pbDataExpected   = NULL;
    DRM_DWORD             cbRead           = 0;
    DRM_DWORD            *pcbRead          = &cbRead;
    DRM_DWORD             cbReadExpected   = 0;

    /*
    ** Set up f_pbData and pbDataExpected
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        if ( argc == 0 || strncmp( argv[0], "DEFAULT", 7 ) == 0 )
        {
            cbData = 10;
            ChkMem( pbDataExpected = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
            DRM_BYT_SetBytes( pbDataExpected, 0, cbData, 0x01 );
        }
        else
        {
            cbData = ( DRM_DWORD )strlen( argv[0] );
            ChkMem( pbDataExpected = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
            MEMCPY( pbDataExpected, argv[0], cbData );
        }
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
        ZEROMEM( pbData, cbData );
    }

    /*
    ** Reset cbData if necessary
    */
    if ( argc > 1 && argv[1] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( localmin( cbData, ( DRM_DWORD )strtoul( argv[1], NULL, 0 ) ), &cbData ) );
    }

    /*
    ** Set up pcbRead
    */
    cbReadExpected = cbData;
    if ( argc > 2 )
    {
        if ( argv[2] == NULL )
        {
            pcbRead = NULL;
        }
        else
        {
            cbReadExpected = ( DRM_DWORD )strtoul( argv[2], NULL, 0 );
        }
    }

    /*
    ** Set up DST
    */
    if ( argc < 4 || argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 5 || argv[4] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }


    ChkDR( DRM_DST_SlotRead( pDST,
                             pSlotContext,
                             cbData,
                             pbData,
                             pcbRead ) );

    /*
    ** Compare with expected values
    */
    if ( pbData != NULL )
    {
        ChkBOOL( MEMCMP( pbData, pbDataExpected, cbData ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if ( pcbRead != NULL )
    {
        ChkBOOL( *pcbRead == cbReadExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbDataExpected );
    return dr;
}

/********************************************************************************************
** Function:    TestDatastoreSlotWrite
**
** Synopsis:    Read from a slot using the given or default parameters.
**
** Arguments:   argv[0] : f_pbData (NULL, DEFAULT, or from input) - Optional. Defaults to all 0x01, 10 bytes.
**              argv[1] : f_cbData - Optional. Defaults to full length of f_pbData.
**              argv[2] : f_pcbWritten - Optional. Defaults to f_cbData, otherwise specifies NULL or expected bytes written.
**              argv[3] : f_pDST (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**              argv[4] : f_pContextSlot (NORMAL or NULL) - Optional. No parameters means use NORMAL.
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreSlotWrite ( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr                = DRM_SUCCESS;
    DRM_DST              *pDST              = NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext      = NULL;
    DRM_DWORD             cbData            = 0;
    DRM_BYTE             *pbData            = NULL;
    DRM_DWORD             cbWritten         = 0;
    DRM_DWORD            *pcbWritten        = &cbWritten;
    DRM_DWORD             cbWrittenExpected = 0;

    /*
    ** Set up f_pbData
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        if ( argc == 0 || strncmp( argv[0], "DEFAULT", 7 ) == 0 )
        {
            cbData = 10;
            ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
            DRM_BYT_SetBytes( pbData, 0, cbData, 0x01 );
        }
        else
        {
            cbData = ( DRM_DWORD )strlen( argv[0] );
            ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbData ) );
            MEMCPY( pbData, argv[0], cbData );
        }
    }

    /*
    ** Reset cbData if necessary
    */
    if ( argc > 1 && argv[1] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( localmin( cbData, ( DRM_DWORD )strtoul( argv[1], NULL, 0 ) ), &cbData ) );
    }

    /*
    ** Set up pcbWritten
    */
    cbWrittenExpected = cbData;
    if ( argc > 2 )
    {
        if ( argv[2] == NULL )
        {
            pcbWritten = NULL;
        }
        else
        {
            cbWrittenExpected = ( DRM_DWORD )strtoul( argv[2], NULL, 0 );
        }
    }

    /*
    ** Set up DST
    */
    if ( argc < 4 || argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );
    }

    /*
    ** Set up DRM_DST_SLOT_CONTEXT
    */
    if ( argc < 5 || argv[4] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pSlotContext, DST_SLOT_CONTEXT_VAR );
    }


    ChkDR( DRM_DST_SlotWrite( pDST,
                              pSlotContext,
                              cbData,
                              pbData,
                              pcbWritten ) );

    /*
    ** Compare with expected values
    */
    if ( pcbWritten != NULL )
    {
        ChkBOOL( *pcbWritten == cbWrittenExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}


/********************************************************************************************
** Function:    TestDatastoreCheckESTDirtyBit
**
** Synopsis:    Checks that the m_fIsDirty member of the EST context is what is expected.
**
** Arguments:   argv[0] : Expected Value, TRUE or FALSE
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestDatastoreCheckESTDirtyBit( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT             dr        = DRM_SUCCESS;
    DRM_BOOL               fExpected = FALSE;
    DRM_DST               *pDST      = NULL;
    const DRM_EST_CONTEXT *pEST      = NULL;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL );

    MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );

    ChkDR( StringToBool( argv[0], &fExpected ) );

    ChkArg( pDST->eImplementationType == eDRM_DST_EMBEDDED );

    pEST = (const DRM_EST_CONTEXT *) pDST->poContextDST;

    ChkBOOL( pEST->m_fIsDirty == fExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PreTestCase
**
** Synopsis:    Pretest case for the DST test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE( DATASTORE )( long lTCID, const char *strTCName )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_DST                     *pDst           = NULL;
    DRM_DST_CONTEXT             *pDstContext    = NULL;
    DRM_DST_NAMESPACE_CONTEXT   *pNsContexts    = NULL;
    DRM_DST_ENUM_CONTEXT        *pEnumContext   = NULL;
    DRM_DST_SLOT_CONTEXT        *pSlotContext   = NULL;
    DRM_DST_SLOT_HINT           *pSlotHint      = NULL;
    DRM_BYTE                    *pbStoreData    = NULL;
    DRM_DWORD                   *pcbStoreData   = NULL;

    /*
    ** Set shared variables
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pDst,         DST_VAR,                   SIZEOF( DRM_DST ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pDstContext,  DST_CONTEXT_VAR,           SIZEOF( DRM_DST_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pNsContexts,  DST_NAMESPACE_CONTEXT_VAR, MAX_NAMESPACE_NUM * SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pEnumContext, DST_ENUM_CONTEXT_VAR,      SIZEOF( DRM_DST_ENUM_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotContext, DST_SLOT_CONTEXT_VAR,      SIZEOF( DRM_DST_SLOT_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSlotHint,    DST_SLOT_HINT_VAR,         SIZEOF( DRM_DST_SLOT_HINT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbStoreData,  DST_PBSTOREDATA_VAR,       DST_TEST_STORE_MAX );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbStoreData, DST_PCBSTOREDATA_VAR,      SIZEOF( DRM_DWORD ) );
    *pcbStoreData = DST_TEST_STORE_MAX;

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PostTestCase
**
** Synopsis:    Posttest case for the DST test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( DATASTORE )( long lTCID, const char *strTCName )
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DST    *pDST  = NULL;
    DRM_WCHAR  *pwszStoreName = NULL;
    DRM_DWORD  *pcchStoreName = 0;

    /*
    ** Attempt to close and uninit the store
    */
    MAP_TO_GLOBAL_TEST_VAR( pDST, DST_VAR );

    ( DRM_VOID )DRM_DST_CloseStore( pDST );
    ( DRM_VOID )DRM_DST_Uninit( pDST );

    /*
    ** Clear Block Header Cache between test cases
    */
    if( DRM_HBHC_IsHdsBlockHeaderCacheSupported() )
    {
        Oem_Hds_ClearBlockHeaderCache( );
    }

    /*
    ** Attempt to remove the store file if it exists
    */
    MAP_TO_GLOBAL_TEST_VAR( pwszStoreName, DST_PBSTOREDATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcchStoreName, DST_PCBSTOREDATA_VAR );
    *pcchStoreName *= SIZEOF( DRM_WCHAR );
    if ( *pcchStoreName > 0 )
    {
        ( DRM_VOID )tRemoveFileW( pwszStoreName );
    }

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( DATASTORE )
    API_ENTRY( TestDatastoreInit )
    API_ENTRY( TestDatastoreUninit )
    API_ENTRY( TestDatastoreCreateStore )
    API_ENTRY( TestDatastoreOpenStore )
    API_ENTRY( TestDatastoreCloseStore )
    API_ENTRY( TestDatastoreCommitStore )
    API_ENTRY( TestDatastoreCleanupStore )
    API_ENTRY( TestDatastoreOpenNamespace )
    API_ENTRY( TestDatastoreCloseNamespace )
    API_ENTRY( TestDatastoreDeleteNamespace )
    API_ENTRY( TestDatastoreCommitNamespace )
    API_ENTRY( TestDatastoreOpenSlot )
    API_ENTRY( TestDatastoreMakeSlotHint )
    API_ENTRY( TestDatastoreCloseSlot )
    API_ENTRY( TestDatastoreDeleteSlot )
    API_ENTRY( TestDatastoreInitSlotEnum )
    API_ENTRY( TestDatastoreSlotEnumNext )
    API_ENTRY( TestDatastoreSlotEnumReloadCurrent )
    API_ENTRY( TestDatastoreSlotEnumDeleteCurrent )
    API_ENTRY( TestDatastoreSlotResize )
    API_ENTRY( TestDatastoreSlotSeek )
    API_ENTRY( TestDatastoreSlotRead )
    API_ENTRY( TestDatastoreSlotWrite )
    API_ENTRY( TestDatastoreCheckESTDirtyBit )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
