/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmhdsblockheadercache.h>


ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    TestManagerClearHDSCaches
**
** Synopsis:
**
** Arguments:   argv[0] : ignored
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerClearHDSCaches( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr         = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD  dwMetaData = 0;
    DRM_DWORD  dwBlockNum = 1;

    ChkArg( argc == 0 );

    if( !DRM_HBHC_IsHdsBlockHeaderCacheSupported() )
    {
        Log( "Trace", "DRMManager does not use cached block headers" );
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /*
    ** Add some metadata to the block header cache
    */
    ChkBOOL( DRM_SUCCEEDED( Oem_Hds_UpdateBlockHeaderCache( dwBlockNum, NULL, 100, NULL, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Make sure the cache is stored
    */
    ChkBOOL( DRM_SUCCEEDED( Oem_Hds_CheckBlockHeaderCache( dwBlockNum, NULL, &dwMetaData, NULL, 0 ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwMetaData == 100, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Now clear the cache
    */
    Drm_ClearAllCaches();

    /*
    ** Make sure the cache is cleared
    */
    ChkBOOL( Oem_Hds_CheckBlockHeaderCache( dwBlockNum, NULL, &dwMetaData, NULL, 0 ) == DRM_E_DST_BLOCK_CACHE_MISS, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** At this point, the test passes
    */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
