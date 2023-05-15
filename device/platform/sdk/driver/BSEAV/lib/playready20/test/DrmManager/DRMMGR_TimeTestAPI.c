/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    Test_ChangeTimeSeconds
**
** Synopsis:    Changes the device clock by seconds
**              
**
** Arguments:   argv[0]             -- offset in seconds, positive to go 
**                                     forward, negative to rollback
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid
*****************************************************************************/
DRM_RESULT DRM_CALL Test_ChangeTimeSeconds( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_LONG   lOffset = 0;
 
    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    lOffset = atol( argv[0] );

    tChangeSystemTime( lOffset );

#ifdef WINCE_TEST
    Sleep( 2000 );
#endif

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_SetFlagOEMTimeOffsetIsAbsolute
**
** Synopsis:    Sets g_fOEMTimeOffsetIsAbsolute from oemtime.c
**              
**
** Arguments:   argv[0]             -- value to set: TRUE or FALSE
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid
*****************************************************************************/
DRM_RESULT DRM_CALL Test_SetFlagOEMTimeOffsetIsAbsolute( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BOOL   fSet    = TRUE;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    if ( DRMCRT_strncmp( argv[0], "TRUE", 4 ) != 0 )
    {
        ChkArg( DRMCRT_strncmp( argv[0], "FALSE", 5 ) == 0 );
        fSet = FALSE;
    }

    tSetFlagOEMTimeOffsetIsAbsolute( fSet );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetMachineClock
**
** Synopsis:    Set the machine clock to a different year             
**
** Arguments:   argv[0]             -- year to set the device clock
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetMachineClock( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( tChangeYearOnMachineClock( (DRM_WORD)atol( argv[0] ) ) );

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
