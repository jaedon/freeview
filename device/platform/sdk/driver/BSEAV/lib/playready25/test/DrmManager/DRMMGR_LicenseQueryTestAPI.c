/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmmathsafe.h>

static const DRM_WCHAR playonpcstring1 []        = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR backupstring1[]           = { ONE_WCHAR('B', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR burntocdstring1 []        = { ONE_WCHAR('P', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR createpmlicensestring1 [] = { ONE_WCHAR('C', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR badstring []              = { ONE_WCHAR('B', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR foostring []              = { ONE_WCHAR('F', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('\0', '\0') };

ENTER_PKTEST_NAMESPACE_CODE;

/*****************************************************************************
** Function:    TestManagerGetLicenseData
**
** Synopsis:    get license data
**
** Arguments:   argv[0] : Data to use in context if not using shared context
**              argv[1] : Actions
**                         "1" - 3 valid actions,
**                         "2" - includes invalid action,
**                         "3" - one valid action,
**                         "4" - one custom action,
**                         other - NULL action
**              argv[2] : if NULL, use NULL license state data
**              argv[3] : Number of actions queried (0 is default)
**              argv[4] : Flags
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetLicenseData( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT *pLocalManagerContext = NULL;
    DRM_DWORD        dwNumActionsQueried  = 0;
    DRM_LONG         iOption              = 0;
    DRM_LONG         iActionOption        = 0;

#define DRM_RIGHTS_STRING_COUNT 3
    DRM_LICENSE_STATE_DATA        pStateData[DRM_RIGHTS_STRING_COUNT] = {0};
    const DRM_CONST_STRING      **ppwszAction                         = NULL;

    static const DRM_CONST_STRING PLAY_ON_PC_STRING1        = CREATE_DRM_STRING( playonpcstring1);
    static const DRM_CONST_STRING BACKUP_STRING1            = CREATE_DRM_STRING(backupstring1 );
    static const DRM_CONST_STRING BURN_TO_CD_STRING1        = CREATE_DRM_STRING(burntocdstring1);
    static const DRM_CONST_STRING CREATE_PM_LICENSE_STRING1 = CREATE_DRM_STRING(createpmlicensestring1);
    static const DRM_CONST_STRING BAD_STRING                = CREATE_DRM_STRING(badstring);
    static const DRM_CONST_STRING FOO_STRING                = CREATE_DRM_STRING(foostring);

    /*
    ** three valid actions
    */
    const DRM_CONST_STRING *DRM_RIGHT_BITS_TO_STRING_1[DRM_RIGHTS_STRING_COUNT] =
    {
        &PLAY_ON_PC_STRING1,
        &CREATE_PM_LICENSE_STRING1,
        &BURN_TO_CD_STRING1,
    };
    /*
    ** include invalid actions
    */
    const DRM_CONST_STRING *DRM_RIGHT_BITS_TO_STRING_2[DRM_RIGHTS_STRING_COUNT] =
    {
        &PLAY_ON_PC_STRING1,
        &BAD_STRING,
        &BACKUP_STRING1,
    };
    /*
    ** only one valid action
    */
    const DRM_CONST_STRING *DRM_RIGHT_BITS_TO_STRING_3[1] =
    {
        &PLAY_ON_PC_STRING1,
    };
    /*
    ** only one customized action named foo
    */
    const DRM_CONST_STRING *DRM_RIGHT_BITS_TO_STRING_4[1] =
    {
        &FOO_STRING,
    };

    /*
    ** at least 5 arguments. The last one is used to decide whether we need to overwrite the context
    */
    if( argc < 5 )
    {
        Log("Trace", "\t\tFrom TestManagerGetLicenseData: error in number of arguments.");
        ChkDR(DRM_E_TEST_INVALIDARG);
    }

    if( !IsDigitalOnlyString( argv[3] ) || !IsDigitalOnlyString( argv[4] ) )
    {
        Log("Trace", "\t\tFrom TestManagerGetLicenseData: error in the argument.");
        ChkDR(DRM_E_TEST_INVALIDARG);
    }

    iOption = atol( argv[4] );
    /*
    ** manager context
    */
    if ( iOption&1 )
    {
        /*
        ** Context is from the input
        */
        if ( argv[0] != NULL )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &cch ) );
            ChkMem( pLocalManagerContext = (DRM_APP_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
            MEMSET( pLocalManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );
            MEMCPY( pLocalManagerContext->rgbOpaqueBuffer, argv[0], min( cch + 1, DRM_APP_CONTEXT_BUFFER_SIZE ) );
        }
    }
    else
    {
        /*
        ** Use shared context
        */
        GET_SHARED_APP_CONTEXT( pLocalManagerContext );
    }

    /*
    ** num actions queried
    */
    if ( argv[3] != NULL )
    {
        dwNumActionsQueried = (DRM_DWORD)atol( argv[3] );
    }

    ChkBOOL( dwNumActionsQueried <= DRM_RIGHTS_STRING_COUNT, DRM_E_FAIL );

    if ( argv[1] != NULL )
    {
        iActionOption = atol( argv[1] );
    }

    switch ( iActionOption )
    {
    case 1:
        ppwszAction = DRM_RIGHT_BITS_TO_STRING_1;
        break;
    case 2:
        ppwszAction = DRM_RIGHT_BITS_TO_STRING_2;
        break;
    case 3:
        ppwszAction = DRM_RIGHT_BITS_TO_STRING_3;
        break;
    case 4:
        ppwszAction = DRM_RIGHT_BITS_TO_STRING_4;
        break;
    default:
        ppwszAction = NULL;
    }

    ChkDR( Drm_LicenseQuery_GetState( pLocalManagerContext,
                                      ppwszAction,
                                      dwNumActionsQueried,
                                      argv[2] ? pStateData : NULL,
                                     (DRMPFNPOLICYCALLBACK)OPLCallback,
                                      NULL ) );
ErrorExit:
    if ( iOption&1 )
    {
        SAFE_OEM_FREE( pLocalManagerContext );
    }
    return dr;
}

/*****************************************************************************
** Function:    TestManagerGetLicenseState
**
** Synopsis:    Wrapper for Drm_LicenseQuery_IsAllowed
**
** Arguments:   argv[0] : Query
**              argv[1] : expected license state category
**              argv[2] : bool indicating if a callback function should be used
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetLicenseState( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr                    = DRM_SUCCESS;
    DRM_CONST_STRING           pdstrQuery            = {0};
    DRM_LICENSE_STATE_CATEGORY iLicenseStateCategory = DRM_LICENSE_STATE_NORIGHT;
    DRM_BOOL                   fUseCallback          = FALSE;
    DRM_APP_CONTEXT           *poManagerContext      = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    if( argc > 0 && argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &pdstrQuery, argv[0] ) );
    }

    if( argc > 2 && argv[2] != NULL )
    {
        ChkDR( StringToBool( argv[2], &fUseCallback ) );
    }

    ChkDR( Drm_LicenseQuery_IsAllowed( poManagerContext,
                                       &pdstrQuery,
                                       fUseCallback ? &OPLCallback : NULL,
                                       NULL,
                                       &iLicenseStateCategory ) );

    if( argc > 1 && argv[1] != NULL )
    {
        ChkBOOL( iLicenseStateCategory == ( DRM_LICENSE_STATE_CATEGORY )atoi( argv[1] ),
                 DRM_E_TEST_LICENSE_STATE_MISMATCH );
    }

ErrorExit:
    FREEDRMSTRING( pdstrQuery );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
