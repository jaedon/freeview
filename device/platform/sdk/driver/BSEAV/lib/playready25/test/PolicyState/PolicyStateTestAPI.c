/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PolicyStateTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

/*********************************************************************
**
** Function: TestPolicyGetStateData
**
** Synopsis: Function that tests the retrieving of policy state data.
**           argv[ 0 ] is used to specify the B64 encoded KID.
**           argv[ 1 ] is used to specify the state name.
**           argv[ 2 ] is used to specify the size of the state data buffer.
**           argv[ 3 ] is used to specify whether the state data buffer is to be NULL.
**           argv[ 4 ] is used to specify the expected result type. 0: DRM_LONG, 1: string.
**           argv[ 5 ] is used to specify the expected result data.
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
DRM_RESULT DRM_CALL TestPolicyGetStateData(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_APP_CONTEXT *poAppContext  = NULL;
    DRM_CONST_STRING dstrKID       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrStateName = EMPTY_DRM_STRING;
    DRM_BYTE        *pbStateData   = NULL;
    DRM_DWORD        cbStateData   = 0;

    ChkArg( argc == 6 );
    ChkArg( argv[ 4 ] != NULL && argv[ 5 ] != NULL );

    if ( argv[ 0 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrKID, argv[ 0 ] ) );
    }

    if ( argv[ 1 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrStateName, argv[ 1 ] ) );
    }

    if ( argv[ 2 ] != NULL )
    {
        cbStateData = strtoul( argv[ 2 ], NULL, 0 );
    }

    if ( argv[ 3 ] != NULL )
    {
        if ( strncmp( argv[ 3 ], "NORMAL", 6 ) == 0 )
        {
            ChkMem( pbStateData = ( DRM_BYTE * )Oem_MemAlloc( cbStateData ) );
        }
    }

    GET_SHARED_APP_CONTEXT( poAppContext );

    ChkDR( Drm_Policy_GetStateData( poAppContext,
                                    &dstrKID,
                                    &dstrStateName,
                                    pbStateData,
                                    &cbStateData ) );

    if ( strncmp( argv[ 4 ], "INTEGER", 7 ) == 0 )
    {
        /*
        ** The expected data type is a DRM_LONG.
        */
        DRM_LONG lData = atol( argv[ 5 ] );

        ChkBOOL( cbStateData == SIZEOF( DRM_LONG ),
                 DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkBOOL( *( ( DRM_LONG * )pbStateData ) == lData,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else if ( strncmp( argv[ 4 ], "STRING", 6 ) == 0 )
    {
        /*
        ** The expected data type is a string.
        */
        DRM_DWORD cbExpected = ( DRM_DWORD )strlen( argv[ 5 ] );

        ChkBOOL( cbStateData == cbExpected,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkBOOL( MEMCMP( pbStateData, argv[ 5 ], cbExpected ) == 0,
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkArg( DRM_E_TEST_INVALIDARG );
    }

ErrorExit:

    SAFE_OEM_FREE( pbStateData );
    FREEDRMSTRING( dstrStateName );
    FREEDRMSTRING( dstrKID );

    return dr;
}

/*********************************************************************
**
** Function: TestPolicySetStateData
**
** Synopsis: Function that tests the storing of policy state data.
**
** Arguments:
**
**           argv[ 0 ] is used to specify the B64 encoded KID.
**           argv[ 1 ] is used to specify the state name.
**           argv[ 2 ] is used to specify the state data type. 0: DRM_LONG, 1: string.
**           argv[ 3 ] is used to specify the state data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestPolicySetStateData(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_APP_CONTEXT *poAppContext  = NULL;
    DRM_CONST_STRING dstrKID       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrStateName = EMPTY_DRM_STRING;
    DRM_BYTE        *pbStateData   = NULL;
    DRM_DWORD        cbStateData   = 0;
    DRM_LONG         lData         = 0;

    ChkArg( argc == 4 );
    ChkArg( argv[ 2 ] != NULL );

    if ( argv[ 0 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrKID, argv[ 0 ] ) );
    }

    if ( argv[ 1 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrStateName, argv[ 1 ] ) );
    }

    if ( strncmp( argv[ 2 ], "INTEGER", 7 ) == 0 )
    {
        /*
        ** The expected data type is a DRM_LONG.
        */
        lData = atol( argv[ 3 ] );

        pbStateData = ( DRM_BYTE * )&lData;
        cbStateData = SIZEOF( DRM_LONG );
    }
    else if ( strncmp( argv[ 2 ], "STRING", 6 ) == 0 )
    {
        /*
        ** The expected data type is a string.
        */
        if ( argv[ 3 ] != NULL )
        {
            pbStateData = ( DRM_BYTE * )argv[ 3 ];

            cbStateData = ( DRM_DWORD )strlen( argv[ 3 ] );
        }
    }
    else
    {
        ChkArg( DRM_E_TEST_INVALIDARG );
    }

    GET_SHARED_APP_CONTEXT( poAppContext );

    ChkDR( Drm_Policy_SetStateData( poAppContext,
                                    &dstrKID,
                                    &dstrStateName,
                                    pbStateData,
                                    cbStateData ) );

ErrorExit:

    FREEDRMSTRING( dstrStateName );
    FREEDRMSTRING( dstrKID );

    return dr;
}

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( PolicyState )

BEGIN_TEST_API_LIST( PolicyState )
    API_ENTRY( TestPolicyGetStateData )
    API_ENTRY( TestPolicySetStateData )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
