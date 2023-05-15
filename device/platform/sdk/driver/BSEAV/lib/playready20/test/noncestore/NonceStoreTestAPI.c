/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmnoncestore.h"
#include <NonceStoreTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE


/*
** Max size of the store data buffer that is
** big enough to store NST_TEST_NONCE_STORE_TOKEN_MAX
** XMR licenses.
*/
#define NST_TEST_NONCE_STORE_BUFFER_MAX      1024

/* Max number of nonces to cache. */
#define NST_TEST_NONCE_CACHE_MAX    10

/* Array of DRM_IDs to store the nonces. */
static DRM_LID g_oNonces[ NST_TEST_NONCE_CACHE_MAX ];

/*********************************************************************
**
** Function: TestNonceStoreOpen
**
** Synopsis: Function that tests the opening of a nonce store.
**           argv[ 0 ] is used to specify the nonce store context.
**           argv[ 1 ] is used to specify the nonce store buffer.
**           argv[ 2 ] is used to specify the size of the nonce store buffer.
**           argv[ 3 ] is used to specify the license store context.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestNonceStoreOpen(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;
    DRM_LICSTORE_CONTEXT *poLicStoreContext = NULL;
    DRM_BYTE *pbStoreData = NULL;
    DRM_DWORD cbStoreData = NST_TEST_NONCE_STORE_BUFFER_MAX;

    if ( argc == 0 || argv[ 0 ] != NULL )
    {
        if ( argc > 0 && strncmp( argv[ 0 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poNST, NST_VAR );
        }
    }

    if ( argc > 1 && argv[ 1 ] != NULL )
    {
        if ( strncmp( argv[ 1 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( pbStoreData, NST_PBSTOREDATA_VAR );
            ZEROMEM( pbStoreData, NST_TEST_NONCE_STORE_BUFFER_MAX );
        }
    }
    
    if ( argc > 2 && argv[ 2 ] != NULL )
    {
        cbStoreData = strtoul( argv[ 2 ], NULL, 0 );
    }
        
    if ( argc > 3 && argv[ 3 ] != NULL )
    {
        if ( strncmp( argv[ 3 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poLicStoreContext, LICSTORE_CONTEXT_VAR );
        }
    }

    ChkDR( DRM_NST_Open( NULL,
                         pbStoreData,
                         cbStoreData,
                         poLicStoreContext,
                         poNST ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: TestNonceStoreClose
**
** Synopsis: Function that tests the closing of a nonce store.
**           argv[ 0 ] is used to specify the nonce store context.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestNonceStoreClose(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;

    if ( argc == 0 || argv[ 0 ] != NULL )
    {
        if ( argc > 0 && strncmp( argv[ 0 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poNST, NST_VAR );
        }
    }

    ChkDR( DRM_NST_Close( poNST ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: TestNonceStoreGenerateNonce
**
** Synopsis: Function that tests the generation of a nonce store token.
**           The returned nonce store token is added to the global DRM_ID
**           array to be used by other test APIs.
**           argv[ 0 ] is used to specify the nonce store context.
**           argv[ 1 ] is used to specify the index of the global DRM_ID array
**           slot that is used to store the generated nonce.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestNonceStoreGenerateNonce(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;
    DRM_ID oNonce = { 0 };
    DRM_DWORD iSlot = NST_TEST_NONCE_CACHE_MAX;

    if ( argc == 0 || argv[ 0 ] != NULL )
    {
        if ( argc > 0 && strncmp( argv[ 0 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poNST, NST_VAR );
        }
    }

    if ( argc > 1 && argv[ 1 ] != NULL )
    {
        iSlot = atoi( argv[ 1 ] );
        
        ChkBOOL( iSlot < NST_TEST_NONCE_CACHE_MAX, DRM_E_FAIL );
    }

    ChkDR( DRM_NST_GenerateNonce( poNST, &oNonce ) );
    
    if ( iSlot < NST_TEST_NONCE_CACHE_MAX )
    {
        MEMCPY( &g_oNonces[ iSlot ], &oNonce, SIZEOF( DRM_LID ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: TestNonceStoreAddLicense
**
** Synopsis: Function that tests the adding of a license to the nonce store.
**           Commit a nonce store token means the increase its usage count.
**           argv[ 0 ] is used to specify the nonce store context.
**           argv[ 1 ] is used to specify file that contains the license
**           to be added.
**           argv[ 2 ] is used to specify the index of the global DRM_ID array
**           slot that is used as the LID to test DRM_NST_AddLicense().
**           argv[ 3 ] is the B64 encoded KID to test DRM_NST_AddLicense().
**           argv[ 4 ] is the B64 encoded LID to test DRM_NST_AddLicense().
**           It is only used if argv[2] is NULL.
**           argv[ 5 ] is the priority value to test DRM_NST_AddLicense().
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestNonceStoreAddLicense(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;
    DRM_BYTE *pbTemp = NULL;
    DRM_DWORD cbTemp = 0;
    DRM_BYTE *pbLicense = NULL;
    DRM_DWORD cbLicense = 0;
    DRM_ID oKID = { 0 };
    DRM_ID oLID = { 0 };
    DRM_DWORD cbKID = DRM_ID_SIZE, cbLID = DRM_ID_SIZE;
    DRM_SUBSTRING dasstrKID = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrLID = EMPTY_DRM_SUBSTRING;
    DRM_DWORD dwPriority = 0;
    DRM_DWORD iSlot = NST_TEST_NONCE_CACHE_MAX;

    if ( argc == 0 || argv[ 0 ] != NULL )
    {
        if ( argc > 0 && strncmp( argv[ 0 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poNST, NST_VAR );
        }
    }

    if ( argc > 1 && argv[ 1 ] != NULL )
    {
        ChkBOOL( LoadTestFile( "data", argv[ 1 ], &pbTemp, &cbTemp ),
                 DRM_E_FAIL );

        cbLicense = cbTemp + SIZEOF( DRM_DWORD );                 
        ChkMem( pbLicense = ( DRM_BYTE * )Oem_MemAlloc( cbLicense ) );
        ZEROMEM( pbLicense, cbLicense );

        /*
        ** The first DWORD of pbLicense is used to store the priority
        ** value.
        */        
        MEMCPY( pbLicense + SIZEOF( DRM_DWORD ), pbTemp, cbTemp );
    }

    if ( argc > 2 && argv[ 2 ] != NULL )
    {
        iSlot = atoi( argv[ 2 ] );
        
        ChkBOOL( iSlot < NST_TEST_NONCE_CACHE_MAX, DRM_E_FAIL );
        
        MEMCPY( &oLID, &g_oNonces[ iSlot ], SIZEOF( DRM_LID ) );
    }

    if ( argc > 3 && argv[ 3 ] != NULL )
    {
        dasstrKID.m_ich = 0;
        dasstrKID.m_cch = ( DRM_DWORD )strlen( argv[ 3 ] );

        ChkDR( DRM_B64_DecodeA( argv[ 3 ],
                                &dasstrKID,
                                &cbKID,
                                ( DRM_BYTE * )&oKID,
                                0 ) );
    }

    if ( argc > 4 && argv[ 4 ] != NULL && iSlot != NST_TEST_NONCE_CACHE_MAX)
    {
        dasstrLID.m_ich = 0;
        dasstrLID.m_cch = ( DRM_DWORD )strlen( argv[ 4 ] );

        ChkDR( DRM_B64_DecodeA( argv[ 4 ],
                                &dasstrLID,
                                &cbLID,
                                ( DRM_BYTE * )&oLID,
                                0 ) );
    }

    if ( argc > 5 && argv[ 5 ] != NULL )
    {
        dwPriority = atoi( argv[ 5 ] );
    }

    ChkDR( DRM_NST_AddLicense( poNST,
                               cbLicense,
                               pbLicense,
                               &oKID,
                               &oLID,
                               dwPriority ) );

ErrorExit:

    SAFE_OEM_FREE( pbTemp );

    SAFE_OEM_FREE( pbLicense );

    return dr;
}

/*********************************************************************
**
** Function: TestNonceStoreVerify
**
** Synopsis: Function that verifies that whether the passed in license
**           and LID match the license and nonce in the nonce store.
**           argv[ 0 ] is used to specify the nonce store context.
**           argv[ 1 ] is used to specify file that contains the license
**           to be verified.
**           argv[ 2 ] is used to specify the index of the global DRM_ID array
**           slot that is used as the LID of the expected license.
**           argv[ 3 ] is the B64 encoded KID of the expected license.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestNonceStoreVerify(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;
    DRM_BYTE *pbLicense1 = NULL;
    DRM_DWORD cbLicense1 = 0;
    DRM_BYTE *pbLicense2 = NULL;
    DRM_DWORD cbLicense2 = 0;
    DRM_ID oKID = { 0 };
    DRM_ID oLID = { 0 };
    DRM_DWORD cbKID = 0;
    DRM_SUBSTRING dasstrKID = EMPTY_DRM_SUBSTRING;
    DRM_DWORD iSlot = NST_TEST_NONCE_CACHE_MAX;
    DRM_NONCESTORE_CONTEXT_INTERNAL *poNSTInternal = NULL;

    if ( argc == 0 || argv[ 0 ] != NULL )
    {
        if ( argc > 0 && strncmp( argv[ 0 ], "NORMAL", 6 ) == 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR( poNST, NST_VAR );
        }
    }

    if ( argc > 1 && argv[ 1 ] != NULL )
    {
        ChkBOOL( LoadTestFile( "data", argv[ 1 ], &pbLicense1, &cbLicense1 ),
                 DRM_E_FAIL );
    }

    if ( argc > 2 && argv[ 2 ] != NULL )
    {
        iSlot = atoi( argv[ 2 ] );
        
        ChkBOOL( iSlot < NST_TEST_NONCE_CACHE_MAX, DRM_E_FAIL );
        
        MEMCPY( &oLID, &g_oNonces[ iSlot ], SIZEOF( DRM_LID ) );
    }

    if ( argc > 3 && argv[ 3 ] != NULL )
    {
        dasstrKID.m_cch = ( DRM_DWORD )strlen( argv[ 3 ] );

        ChkDR( DRM_B64_DecodeA( argv[ 3 ],
                                &dasstrKID,
                                &cbKID,
                                ( DRM_BYTE * )&oKID,
                                0 ) );
    }

    poNSTInternal = ( DRM_NONCESTORE_CONTEXT_INTERNAL * )poNST;

    ChkBOOL( MEMCMP( &poNSTInternal->m_oToken, &oLID, SIZEOF( DRM_ID ) ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr =  DRM_LST_GetLicense( poNSTInternal->m_poLicStoreContext,
                              &oKID,
                              &oLID,
                              NULL,
                              NULL, 
                              &cbLicense2 );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkBOOL( cbLicense1 + SIZEOF( DRM_DWORD ) == cbLicense2, DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkMem( pbLicense2 = ( DRM_BYTE * )Oem_MemAlloc( cbLicense2 ) );
        
        ChkDR( DRM_LST_GetLicense( poNSTInternal->m_poLicStoreContext,
                                   &oKID,
                                   &oLID,
                                   NULL,
                                   pbLicense2, 
                                   &cbLicense2 ) );
                                   
        ChkBOOL( MEMCMP( pbLicense1, pbLicense2, cbLicense1 ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkDR( dr );
    }
    

ErrorExit:

    SAFE_OEM_FREE( pbLicense1 );

    SAFE_OEM_FREE( pbLicense2 );

    return dr;
}

/*********************************************************************
**
** Function: PreTestCase
**
** Synopsis: Function that initializes the environment before the running
**           of each test case.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE( NonceStore )(
    long lTCID,
    const char *strTCName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_NONCESTORE_CONTEXT *poNST = NULL;
    DRM_LICSTORE_CONTEXT *poLicStoreContext = NULL;
    DRM_BYTE *pbStoreData = NULL;

    MAP_TO_GLOBAL_TEST_VAR_Alloc( poNST, NST_VAR, SIZEOF( DRM_NONCESTORE_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poLicStoreContext, LICSTORE_CONTEXT_VAR, SIZEOF( DRM_LICSTORE_CONTEXT ) );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbStoreData, NST_PBSTOREDATA_VAR, NST_TEST_NONCE_STORE_BUFFER_MAX );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: PostTestCase
**
** Synopsis: Function that cleans up the environment after the running
**           of each test case.
**
**           Any dynamically allocated memory is released here.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( NonceStore )(
    long lTCID,
    const char *strTCName )
{
    DRM_RESULT dr = DRM_SUCCESS;

    return dr;
}

BEGIN_TEST_API_LIST( NonceStore )
    API_ENTRY( TestNonceStoreOpen )
    API_ENTRY( TestNonceStoreClose )
    API_ENTRY( TestNonceStoreGenerateNonce )
    API_ENTRY( TestNonceStoreAddLicense )
    API_ENTRY( TestNonceStoreVerify )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
