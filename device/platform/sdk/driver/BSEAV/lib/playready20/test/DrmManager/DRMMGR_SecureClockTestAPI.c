/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <drmsecureclock.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    iGetSecStorePassword
**
** Synopsis:    Get the secure store password             
**
**
** Arguments:   rgbPassword             -- password output bufer
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
static DRM_RESULT iGetSecStorePassword( DRM_BYTE *rgbPassword )
{
    DRM_RESULT       dr                              = DRM_SUCCESS;
    DRM_BB_CONTEXT  *pBlackboxContext                = NULL;
    DRM_BYTE         rgbBuffer[MAX_DEVICE_CERT_SIZE] = {0};    
    DRM_CONST_STRING dstrDevCert                     = EMPTY_DRM_STRING;

    DSTR_FROM_PB( &dstrDevCert, rgbBuffer, SIZEOF(rgbBuffer) );

    /* 
    ** Get devcert to open the blackbox 
    */
    ChkDR( tGetBBContext( &pBlackboxContext ) );
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pBlackboxContext ) );
    }
    ChkDR( DRM_BBX_Initialize( NULL, pBlackboxContext ) );

    ChkDR( DRM_SST_CreateGlobalStorePassword( rgbPassword, (DRM_BYTE*)pBlackboxContext ) );
ErrorExit:
    if ( pBlackboxContext != NULL )
    {
        tFreeBBContext( pBlackboxContext );
    }
    return dr;
}

/*****************************************************************************
** Function:    _SetSecureClockData
**
** Synopsis:    Sets the secure clock data in the DST             
**
**
** Arguments:   pClock             -- Secure Clock Data
**              pDST               -- The DST
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
static DRM_RESULT _SetSecureClockData( DRM_SECSTORE_CLOCK_DATA *pClock, DRM_DST *pDST )
{
    DRM_RESULT            dr                                     = DRM_SUCCESS;
    DRM_SECSTORE_CONTEXT *pSecStoreContext                       = NULL;
    DRM_BYTE              rgbSecStoreInPswd[DRM_SHA1_DIGEST_LEN] = {0};
    DRM_APP_CONTEXT      *poManagerContext                       = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    if ( pDST == NULL )
    {
        ChkArg( poManagerContext != NULL );
        pDST = &( ( DRM_APP_CONTEXT_INTERNAL* )poManagerContext )->oDatastoreHDS;
    }
    
    ChkMem( pSecStoreContext = (DRM_SECSTORE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_SECSTORE_CONTEXT ) ) );
    ZEROMEM( pSecStoreContext, SIZEOF( DRM_SECSTORE_CONTEXT ) );
    ChkDR( iGetSecStorePassword( rgbSecStoreInPswd ) );
    ChkDR( DRM_SST_SetData( pSecStoreContext,
                            &g_idSData,
                            &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            pDST,
                            (DRM_BYTE*)pClock, 
                            sizeof(DRM_SECSTORE_CLOCK_DATA) ) );

ErrorExit:
    SAFE_OEM_FREE( pSecStoreContext );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetSecureClock
**
** Synopsis:    Sets the secure clock data directly             
**
**
** Arguments:   argv[0]             -- validity flag
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetSecureClock( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    DRM_SECSTORE_CLOCK_DATA oClock      = {0};
    DRMSYSTEMTIME           oSystemTime = {0};
    DRM_DST                *pDST        = NULL;
    DRM_CONST_STRING        dstrHdsPath = EMPTY_DRM_STRING;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );
    
    oClock.flag = (DRM_WORD)atol( argv[0] );

    Oem_Clock_GetSystemTime( NULL, &oSystemTime );
    ChkArg( Oem_Clock_SystemTimeToFileTime( &oSystemTime, (DRMFILETIME*)&( oClock.LastKnownRealtimeSecureClock ) ) );
    
    ChkDR( tGetDeviceStorePathname( wszPathName, &dstrHdsPath ) );
    ChkDR( OpenHDS( &pDST, dstrHdsPath.pwszString, FALSE ) );

    ChkDR( _SetSecureClockData( &oClock, pDST ) );

ErrorExit:
    CloseHDS( pDST );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetGetClockState
**
** Synopsis:    Set or Verify the OEM secure clock state             
**
**
** Arguments:   argv[0]             -- operation: Set, Verify
**              argv[1]             -- DRM_BOOL value to set or the expected DRM_BOOL value
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetGetClockState( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr     = DRM_SUCCESS;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        DRM_BOOL   fState = FALSE;
        DRM_BOOL   fGot   = FALSE;

        ChkArg( argc == 2 );
        ChkArg( argv[0] != NULL );
        ChkArg( argv[1] != NULL );
    
        fState = !strncmp( argv[1], "TRUE", 4 );
    
        if ( !strncmp( argv[0], "Set", 3 ) ) 
        {
            ChkDR( Oem_Clock_SetResetState( NULL, fState ) );
        } 
        else if ( !strncmp( argv[0], "Verify", 6 ) ) 
        {
            ChkDR( Oem_Clock_GetResetState( NULL, &fGot ) );
            if ( fGot != fState )
            {
                dr = DRM_S_FALSE;
            }
        }
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
