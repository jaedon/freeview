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
** Function:    ReportProgress
**
** Synopsis:    Log the progress of cleaning the license store             
**
** Arguments:
**              pvCallerData        -- Caller Data 
**              dwPercentDone       -- Percent of licenses processed
**              cRemainingLicenses  -- Number of licenses remaining
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              DRM_E_DEVICENOTINIT if caller does not initiate
*****************************************************************************/
DRM_RESULT DRM_CALL  ReportProgress(
    __in const DRM_VOID  *pvCallerData, 
    __in       DRM_DWORD  dwPercentDone,
    __in       DRM_DWORD  cRemainingLicenses
    )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    Log("Trace", "\t%d%% complete, %d licenses remaining", dwPercentDone, cRemainingLicenses );
    if ( pvCallerData )
    {
        dr = DRM_E_DEVICENOTINIT;
    }
    
   return dr;
}

/*****************************************************************************
** Function:    TestManagerCleanLicenseStore
**
** Synopsis:    Cleanup of the license store             
**
** Arguments:
**              argv[0]  -- FALSE to have Argv[1] not relative to the 
**                          start time of the test. Other valuses are ignored.
**              argv[1]  -- Adjust the time in seconds (optional )
**              argv[2]  -- Type of clean operation to perform (optional)  
**                          Valid values are:
**                              ALL,
**                              DELETE_EXPIRED_LICENSES,
**                              DELETE_REMOVAL_DATE_LICENSES
**                           Default value is ALL.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerCleanLicenseStore( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_APP_CONTEXT *poManagerContext = NULL;
    long             lAdjTime         = 0;
    DRM_DWORD        dwFlags          = DRM_STORE_CLEANUP_ALL;

    GET_SHARED_APP_CONTEXT( poManagerContext );
    
    /*
    ** Removal dates are created based on the start time of the test case.
    ** Date Calculations are based on that unless specified other wise.
    */
    if ( !( argc > 0 
         && argv[0] != NULL
         && 0 == strncmp( argv[0], "FALSE", 5 ) ) )
    {
        DRMSYSTEMTIME            *pTestStartTime       = NULL;
        DRMFILETIME               ftCurrent            = {0};
        DRMFILETIME               ftTestStartTime      = {0};
        DRM_UINT64                ui64Current          = DRM_UI64LITERAL( 0, 0 );
        DRM_UINT64                ui64TestStartTime    = DRM_UI64LITERAL( 0, 0 );
        DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext;
        
        MAP_TO_GLOBAL_TEST_VAR( pTestStartTime, LICENSE_REMOVAL_DATE_VAR );
        
        Oem_Clock_GetSystemTimeAsFileTime( poAppContextInternal->pOEMContext, &ftCurrent );
        
        if ( DRM_SUCCEEDED( Oem_Clock_SystemTimeToFileTime( pTestStartTime, &ftTestStartTime ) ) )
        {
            FILETIME_TO_UI64( ftCurrent, ui64Current );
            FILETIME_TO_UI64( ftTestStartTime, ui64TestStartTime );

            lAdjTime += DRM_I64ToUI32( DRM_I64Div( DRM_I64Sub( DRM_UI2I64( ui64TestStartTime ), DRM_UI2I64( ui64Current ) ), DRM_I64( C_TICS_PER_SECOND ) ) );
        }
    }

    if( argc >= 2 )
    {
        ChkArg( argv[1] != NULL );
        lAdjTime += strtol( argv[1], NULL, 0 );
    }
    tChangeSystemTime( lAdjTime );

    if ( argc >= 3 && argv[2] != NULL )
    {
        if( strncmp( argv[2], "ALL", 3 ) == 0 )
        {
            dwFlags = DRM_STORE_CLEANUP_ALL;
        }
        else if( strncmp( argv[2], "DELETE_EXPIRED_LICENSES", 23 ) == 0 )
        {
            dwFlags = DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES;
        }
        else if( strncmp( argv[2], "DELETE_REMOVAL_DATE_LICENSES", 28 ) == 0 )
        {
            dwFlags = DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES;
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }
    }

    if ( argv[0] )
    {
        ChkDR( Drm_StoreMgmt_CleanupStore( poManagerContext, 
                                           dwFlags,
                                           NULL,
                                           20,
                                           ReportProgress ) );
    }
    
ErrorExit:
    if( lAdjTime != 0 )
    {
        tChangeSystemTime( -lAdjTime );
    }
    return dr;
}


/*****************************************************************************
** Function:    TestManagerDeleteLicensesByKID
**
** Synopsis:    Deletes licenses by KIDs             
**
** Arguments:
**              argv[0]  -- optional, KID
**              argv[1]  -- optional, expected number of licenses deleted
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerDeleteLicensesByKID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_DWORD        dwDeleted          = 0;
    DRM_CONST_STRING dcstrKID           = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstr             = {0};
    DRM_CHAR        *pstrRandomKeyID    = NULL;
    DRM_APP_CONTEXT *poManagerContext   = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc <= 2 );

    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** ) &pstrRandomKeyID, NULL ) ) )
    {
        pstrRandomKeyID = NULL;
    }

    if( argc >= 1 && argv[0] != NULL )
    {
        if( strncmp( argv[0], "PREGENERATEDRANDOM", 18 ) == 0 
            && pstrRandomKeyID != NULL )
        {
            dcstrKID.cchString = (DRM_DWORD)DRMCRT_strlen( pstrRandomKeyID ) + 1;
            dasstr.m_cch = dcstrKID.cchString;
            ChkMem(dcstrKID.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dcstrKID.cchString * SIZEOF( DRM_WCHAR ) ) );
            ZEROMEM( ( DRM_BYTE * )dcstrKID.pwszString, dcstrKID.cchString * SIZEOF( DRM_WCHAR ) );
            /*
            ** Safe because input parameter has ASCII symbols only 
            */
            DRM_UTL_PromoteASCIItoUNICODE( pstrRandomKeyID, &dasstr, (DRM_STRING *)&dcstrKID ); 
            dcstrKID.cchString--;
        }
        else
        {
            ChkDR( MakeDRMString( &dcstrKID, argv[0] ) );
        }
    }

    ChkDR( Drm_StoreMgmt_DeleteLicenses( poManagerContext,  
              ( argc >= 1 && argv[0] == NULL ) ? NULL : &dcstrKID, 
              ( argc >= 2 && argv[1] == NULL ) ? NULL : &dwDeleted ) );

    if( argc >= 2 && argv[1] != NULL )
    {
        DRM_DWORD dwExpected = 0;

        dwExpected = (DRM_DWORD)atol( argv[1] );

        if( dwExpected != dwDeleted )
        {
            Log("Fail", "Drm_StoreMgmt_DeleteLicenses reports %d licenses deleted. Expected %d", dwDeleted, dwExpected );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

ErrorExit:

    FREEDRMSTRING( dcstrKID );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
