/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmdla.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    TestManagerGetDeviceProperty
**
** Synopsis:    Test wrapper function for Drm_Device_GetProperty
**
** Arguments:   argv[0]    : property index
**              argv[1]    : expected property for verification
**              argv[2]    : type of argv[1]; "BYTE" or "WCHAR"
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetDeviceProperty( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_DWORD       *pcbProperty      = NULL;
    DRM_BYTE        *pbProperty       = NULL;
    DRM_CONST_STRING wszProperty      = {0};
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    MAP_TO_GLOBAL_TEST_VAR( pbProperty, PROPERTY_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcbProperty, PROPERTY_SIZE_VAR );

    /*
    ** argv[0] for property index; argv[1] for verify, argv[2] is type of argv[1]: BYTE or WCHAR
    */
    ChkArg( argc >= 2 );
    ChkArg( argc <= 3 );
    ChkArg( argv[0] != NULL );

    dr = Drm_Device_GetProperty( poManagerContext,
                                 (DRM_DEVICE_GET_PROPERTY) atoi( argv[0] ),
                                 pbProperty,
                                 pcbProperty );

    if( DRM_E_BUFFERTOOSMALL == dr )
    {
        MAP_TO_GLOBAL_TEST_VAR_Realloc( pbProperty, PROPERTY_DATA_VAR, *pcbProperty );

        ChkDR( Drm_Device_GetProperty( poManagerContext,
                                       (DRM_DEVICE_GET_PROPERTY) atoi( argv[0] ),
                                       pbProperty,
                                       pcbProperty ) );
    }
    else
    {
        ChkDR(dr);
    }
    /*
    ** verify the property
    */

    if ( pbProperty && argv[1] )
    {
        if ( argc == 3
             && DRMCRT_strlen( argv[2] ) == 4
             && DRMCRT_strncmp( argv[2], "BYTE", 4) == 0 )
        {
            ChkBOOL( !DRMCRT_strncmp( (DRM_CHAR*)pbProperty, argv[1], *pcbProperty ),
                     DRM_E_TEST_UNEXPECTED_DEVICE_PROPERTY );
        }
        else
        {
            ChkDR( MakeDRMString( &wszProperty, argv[1] ) );
            ChkBOOL( !DRMCRT_wcsncmp( (DRM_WCHAR*)pbProperty, wszProperty.pwszString,wszProperty.cchString ),
                     DRM_E_TEST_UNEXPECTED_DEVICE_PROPERTY );
        }
    }

ErrorExit:
    FREEDRMSTRING( wszProperty );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
