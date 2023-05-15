/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <drmdatastore.h>
#include <drmhds.h>
#include <drmremovaldatestore.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    GetLicenseEnum
**
** Synopsis:    
**
** Arguments:   KID              : KID
**              pdwCount         : LID 
**              pLicStoreContext : option in creating a test license
**              ppLicEnumContext : extra xml string to add to the license
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:       *ppLicEnumContext must be NULL
**              Caller must free memory allocated to *ppLicEnumContext
**
******************************************************************************/
DRM_RESULT DRM_CALL GetLicenseEnum( DRM_KID KID, DRM_DWORD* pdwCount, DRM_LICSTORE_CONTEXT* pLicStoreContext, DRM_LICSTOREENUM_CONTEXT** ppLicEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pdwCount != NULL );
    ChkArg( ppLicEnumContext != NULL );
    ChkArg( *ppLicEnumContext == NULL );
    
    ChkMem( *ppLicEnumContext = (DRM_LICSTOREENUM_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_LICSTOREENUM_CONTEXT ) ) );
    MEMSET( *ppLicEnumContext, 0, SIZEOF( DRM_LICSTOREENUM_CONTEXT ) );

    
    ChkDR( DRM_LST_InitEnum( pLicStoreContext, 
                             &KID,
                             FALSE, /* prioritize */
                             *ppLicEnumContext ) );

    ChkDR( DRM_LST_LicCount( *ppLicEnumContext,
                             pdwCount ) );
ErrorExit:  
    
    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
/*****************************************************************************
** Function:    TestEnumLicense
**
** Synopsis:    
**
** Arguments:   argv[0] : KID
**              argv[1] : Store type: XMR or XML 
**              argv[2] : optional, expected number of licenses in the Enum
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestEnumLicense( long argc, __in_ecount( argc ) char **argv )    
{
    DRM_KID                     kid;
    DRM_SUBSTRING               ssKID                = EMPTY_DRM_SUBSTRING;
    DRM_DWORD                   dwSize               = DRM_ID_SIZE;
    DRM_LICSTOREENUM_CONTEXT*   pLicStoreEnumContext = NULL;
    DRM_DWORD                   dwNOLicenses         = 0;
    DRM_DST                     oDst                 = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_DST_CONTEXT             oDstContext          = { 0 };
    DRM_LICSTORE_CONTEXT        oLicStore            = {0};
    DRM_RESULT                  dr                   = DRM_SUCCESS;
    DRM_CONST_STRING           *pwszDeviceStore      = NULL;


    ChkArg( argc > 2 )    
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    GET_SHARED_DEVICE_STORE( pwszDeviceStore );

    ssKID.m_ich = 0;
    ssKID.m_cch = DRMCRT_strlen( argv[0] );
    
    /* 
    ** convert from LPSTR to KID      
    */
    ChkDR( DRM_B64_DecodeA( argv[0],
                              &ssKID,
                              &dwSize,
                              kid.rgb,
                              0 ) );

    ChkDR( DRM_DST_Init( eDRM_DST_HDS, &oDstContext, SIZEOF( DRM_DST_CONTEXT ), &oDst ) );    

    ChkDR( DRM_DST_OpenStore( NULL, 
                ( DRM_BYTE * )pwszDeviceStore->pwszString, 
                              pwszDeviceStore->cchString * SIZEOF( DRM_WCHAR ),
                              0,
                              &oDst ) );

    if ( strncmp( argv[1], "XML", 3 ) == 0 )
    {
        ChkDR( DRM_LST_Open( &oLicStore, &oDst, eDRM_LICENSE_STORE_XML ) );
    }   
    else
    {
        ChkArg( strncmp( argv[1], "XMR", 3 ) == 0 );
        ChkDR( DRM_LST_Open( &oLicStore, &oDst, eDRM_LICENSE_STORE_XMR ) );
    }

    ChkDR( GetLicenseEnum( kid, &dwNOLicenses, &oLicStore, &pLicStoreEnumContext ) );

    if ( ( argc >= 3 ) && ( argv[2] != NULL ) ) /* check the number of licenses expected */
    {
        if ( atoi( argv[2] ) != ( DRM_LONG )dwNOLicenses )
        {
            Log( "Trace", "Number of Licenses (%d) with the KID does not match expected", dwNOLicenses );
            ChkDR( DRM_E_TEST_UNEXPECTED_LICENSE_COUNT );
        }
    }
    
ErrorExit:

    DRM_LST_Close( &oLicStore );
    DRM_DST_CloseStore( &oDst );
    DRM_DST_Uninit( &oDst );

    SAFE_OEM_FREE( pLicStoreEnumContext );
    
    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif

/*****************************************************************************
** Function:    TestManagerGetSourceID
**
** Synopsis:    This method is used to verify the source id in drm manager context.
**
** Arguments:   argv[0] : source id which is expcted to be present in drm manager context.
**              argv[1] : Flag indicating whether NULL should be passed for DRM Manager Context. 
**              argv[2] : Flag indicating whether NULL should be passed for the variable holding 
**                        the sourcid result from the API
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetSourceID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_LONG         iOption1         = 0;
    DRM_LONG         iOption2         = 0;
    DRM_DWORD        dwSourceID       = 0;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    if( argc != 3 )
    {
        Log( "Trace", "\t\tFrom TestManagerGetSourceID: error in argument." );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
    GET_SHARED_APP_CONTEXT( poManagerContext );
    
    iOption1=(DRM_LONG)atol( argv[1] );
    iOption2=(DRM_LONG)atol( argv[2] );

    if( iOption1 && iOption2 )
    {
        dr = Drm_License_GetProperty( poManagerContext, DRM_LGP_SOURCE_ID, NULL, NULL, &dwSourceID );
    }
    else if( !iOption1 && iOption2 )
    {
        dr = Drm_License_GetProperty( NULL, DRM_LGP_SOURCE_ID, NULL, NULL, &dwSourceID );
    }
    else if( iOption1 && !iOption2 )
    {
        dr = Drm_License_GetProperty( poManagerContext, DRM_LGP_SOURCE_ID, NULL, NULL, NULL );
    }
    else if( !iOption1 && !iOption2 )
    {
        dr = Drm_License_GetProperty( NULL, DRM_LGP_SOURCE_ID, NULL, NULL, NULL );
    }

    ChkDR( dr );
    if( atoi( argv[0] ) != ( DRM_LONG )dwSourceID )
    {
        Log( "Trace", "\t\tFrom TestManagerGetSourceID: SourceID mismatch" );
        ChkDR( DRM_E_TEST_SOURCE_ID_MISMATCH );
    }
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: TestManagerIsInclusionGUIDPresent
** 
** Synopsis: This tests Drm_License_GetProperty (inclusion guid).  
** It takes as arguments a GUID argument to pass to the 
** function and an expected output.  It also may take
** optional arguments to indicate testing with a NULL 
** context or NULL buffers.
** 
** Arguments:
**   argc    -- must be at least 2, no greater than 4
**   argv[0] -- The expected output of Drm_License_GetProperty
**   argv[1] -- The GUID to use 
**   argv[2] -- optional flag indicating that a NULL 
**              context should be used (default is FALSE)
**   argv[3] -- optional flag indicating that a NULL
**              output buffer should be used (default is FALSE)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerIsInclusionGUIDPresent( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr               = DRM_SUCCESS;       /* Return value of test function*/
    DRM_LGP_HAS_INCLUSION_GUID_EXTRA_DATA guidArg = {0}; /* Holds the input GUID */
    DRM_DWORD   cbGuidArg        = SIZEOF( guidArg ); /* size, in bytes, of the guidArg struct */
    DRM_BOOL    fExpectedResult  = FALSE;             /* Holds the expected output of Drm_License_GetProperty */
    DRM_DWORD   dwExpectedResult = 0;                 /* Holds the expected output of Drm_License_GetProperty */
    DRM_DWORD   dwResult         = FALSE;             /* Holds the actual output of Drm_License_GetProperty */
    DRM_CONST_STRING dcstrTemp   = EMPTY_DRM_STRING;  /* Holds the input GUID as a DRM_CONST_STRING before translation into DRM_GUID */
    DRM_BOOL    fUseNullContext  = FALSE;             /* Flag for use of a NULL context argument, default to FALSE */
    DRM_BOOL    fUseNullBuffer   = FALSE;             /* Flag for use of a NULL output buffer argument, default to FALSE */
    DRM_APP_CONTEXT *poManagerContext = NULL;

    ChkArg( argc >= 2 );
    ChkArg( argc <= 4 );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    /* 
    ** Parse the expected output 
    */
    ChkDR( StringToBool(argv[0], &fExpectedResult) );
    dwExpectedResult = fExpectedResult ? 1 : 0;

    /* 
    ** Parse the GUID to use 
    */
    ChkDR( MakeDRMString( &dcstrTemp, argv[1] ) );
    ChkDR( DRM_UTL_StringToGuid( &dcstrTemp, &guidArg.guid ) );

    /* 
    ** Check for optional flags 
    */
    if ( argc >= 3 )
    {
        /* 
        ** Set NULL context flag 
        */
        ChkDR( StringToBool(argv[2], &fUseNullContext) );
    }
    if ( argc == 4 )
    {
        /* 
        ** Set NULL output buffer flag 
        */
        ChkDR( StringToBool(argv[3], &fUseNullBuffer) );
    }

    /* 
    ** Call Drm_License_GetProperty with the specified arguments 
    */
    ChkDR( Drm_License_GetProperty( fUseNullContext ? NULL : poManagerContext,
                                    DRM_LGP_HAS_INCLUSION_GUID,
                       (DRM_BYTE *)&guidArg,
                                   &cbGuidArg,
                                    fUseNullBuffer ? NULL : &dwResult ) );

    if ( dwResult != dwExpectedResult )
    {
        dr = DRM_E_FAIL;
    }

ErrorExit:

    FREEDRMSTRING( dcstrTemp );

    return dr;
}

/*******************************************************************************************
** Function:    VerifyEmbeddingBehavior
**
** Synopsis:    Finds a License in the XmrStore and validates that the embedding behavior
**              field is the expected value
**
** Arguments:
**              argv[0] -- KID to Check
**              argv[1] -- LID to Check
**              argv[2] -- Expected Behavior for license ( "IGNORE", "COPY", or "MOVE" )
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
*******************************************************************************************/
#define __cbMaxStack 4096
DRM_RESULT DRM_CALL VerifyEmbeddingBehavior( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr           = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pAppContext  = NULL;
    DRM_LICSTORE_CONTEXT     *poContextXMR = NULL;
    DRM_KID                   oKid         = { 0 };
    DRM_DWORD                 cbKid        = DRM_ID_SIZE;
    DRM_LID                   oLid         = { 0 };
    DRM_BYTE                 *pbLicense    = NULL;
    DRM_DWORD                 cbLicense    = 0;
    DRM_WCHAR                 rgwchAttr[256] = {0};
    DRM_CONST_STRING          dstrKidValue   = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrLidValue   = EMPTY_DRM_STRING;
    DRM_STACK_ALLOCATOR_CONTEXT oStack       = { 0 };
    DRM_BYTE                  rgbStack[__cbMaxStack]   = { 0 };
    DRM_XMR_LICENSE           oXMRLicense              = { 0 };
    DRM_WORD                  wExpectedBehavior        = 0;

    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    GET_SHARED_APP_CONTEXT( pAppContext );

    ChkBOOL( pAppContext->fLicStoreOpenXMR, 
             DRM_E_TEST_LICENSESTORE_NOT_OPEN );
    poContextXMR = &(pAppContext->oLicStoreContextXMR );


    /*
    ** base64 decode the KID argument 
    */
    TST_UTL_mbstowcs( rgwchAttr, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
    dstrKidValue.pwszString = rgwchAttr;
    dstrKidValue.cchString = DRMCRT_strlen( argv[0] );
    ZEROMEM( &oKid, SIZEOF( DRM_KID ) );
    cbKid = SIZEOF( DRM_KID );
    ChkDR( DRM_B64_DecodeW( &dstrKidValue, &cbKid, oKid.rgb, 0 ) );
    ZEROMEM( rgwchAttr, SIZEOF( rgwchAttr ) );

    /*
    ** Convert the LID from a string to a GUID
    */
    TST_UTL_mbstowcs( rgwchAttr, argv[1],  DRMCRT_strlen( argv[1] ) + 1 );
    dstrLidValue.pwszString = rgwchAttr;
    dstrLidValue.cchString= ( DRM_DWORD )DRMCRT_strlen( argv[1] );
    ZEROMEM( &oLid, SIZEOF( DRM_LID ) );
    ChkDR( DRM_UTL_StringToGuid( &dstrLidValue, ( DRM_GUID* )oLid.rgb ) );

    /*
    ** Get the expected value
    */
    if ( strcmp( argv[2], "IGNORE" ) == 0 )
    {
        wExpectedBehavior = XMR_EMBEDDING_BEHAVIOR_IGNORE;
    }
    else if ( strcmp( argv[2], "COPY" ) == 0 )
    {
        wExpectedBehavior = XMR_EMBEDDING_BEHAVIOR_COPY;
    }
    else if ( strcmp( argv[2], "MOVE" ) == 0 )
    {
        wExpectedBehavior = XMR_EMBEDDING_BEHAVIOR_MOVE;
    }

    dr = DRM_LST_GetLicense( poContextXMR, &oKid, &oLid, NULL, NULL, &cbLicense );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbLicense = ( DRM_BYTE * ) Oem_MemAlloc( cbLicense ) );
        ZEROMEM( pbLicense, cbLicense );
        ChkDR( DRM_LST_GetLicense( poContextXMR, &oKid, &oLid, NULL, pbLicense, &cbLicense ) );
    }
    else
    {
        ChkDR( dr );
    }
        
    oStack.nStackTop = 0;
    oStack.pbStack   = rgbStack;
    oStack.cbStack   = __cbMaxStack;
    ZEROMEM( rgbStack, __cbMaxStack );

    ChkDR( DRM_XMR_UnpackLicense( pbLicense, cbLicense, &oStack, &oXMRLicense ) );
    ChkBOOL( XMR_IS_EMBEDDING_BEHAVIOR_VALID( &oXMRLicense ),
             DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( oXMRLicense.containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue
         ==  wExpectedBehavior, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}
#undef __cbMaxStack


/*******************************************************************************************
** Function:    VerifyEmbeddedLicenseAPI
**
** Synopsis:    Verifies the Embedded license in PRO.This API is added for "Add Embedded License to PRO and Retrieve"
**              test scenario in DailyTestDrmManager.pc.pk.ce
**
** Arguments:
**              argv[0] -- KID to Check
**              argv[1] -- LID to Check
**              
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
*******************************************************************************************/
#define __cbMaxStack 4096
DRM_RESULT DRM_CALL VerifyEmbeddedLicenseAPI( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr           = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pAppContext  = NULL;
    DRM_LICSTORE_CONTEXT     *poContextEMBEDDED = NULL;
    DRM_KID                   oKid         = { 0 };
    DRM_DWORD                 cbKid        = DRM_ID_SIZE;
    DRM_LID                   oLid         = { 0 };
    DRM_BYTE                 *pbLicense    = NULL;
    DRM_DWORD                 cbLicense    = 0;
    DRM_WCHAR                 rgwchAttr[256] = {0};
    DRM_CONST_STRING          dstrKidValue   = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrLidValue   = EMPTY_DRM_STRING;
    DRM_BOOL                  fESTOpened     = FALSE;
    
    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    GET_SHARED_APP_CONTEXT( pAppContext );

    
    ChkBOOL( pAppContext->fSecStoreGlobalContextOpen, 
        DRM_E_TEST_LICENSESTORE_NOT_OPEN );
    poContextEMBEDDED = &(pAppContext->oLicStoreContextEmbedded );
        
    /*
    ** base64 decode the KID argument 
    */
    TST_UTL_mbstowcs( rgwchAttr, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
    dstrKidValue.pwszString = rgwchAttr;
    dstrKidValue.cchString = DRMCRT_strlen( argv[0] );
    ZEROMEM( &oKid, SIZEOF( DRM_KID ) );
    cbKid = SIZEOF( DRM_KID );
    ChkDR( DRM_B64_DecodeW( &dstrKidValue, &cbKid, oKid.rgb, 0 ) );
    ZEROMEM( rgwchAttr, SIZEOF( rgwchAttr ) );

    /*
    ** Convert the LID from a string to a GUID
    */
    TST_UTL_mbstowcs( rgwchAttr, argv[1],  DRMCRT_strlen( argv[1] ) + 1 );
    dstrLidValue.pwszString = rgwchAttr;
    dstrLidValue.cchString= ( DRM_DWORD )DRMCRT_strlen( argv[1] );
    ZEROMEM( &oLid, SIZEOF( DRM_LID ) );
    ChkDR( DRM_UTL_StringToGuid( &dstrLidValue, ( DRM_GUID* )oLid.rgb ) );
  
    ChkDR(DRM_DST_OpenStore( pAppContext->pOEMContext,
                             pAppContext->pbEmbeddedStore,
                             pAppContext->cbEmbeddedStore,
                             0,
                             &pAppContext->oDatastoreEST )) ;
    
    ChkDR( DRM_LST_Open( &pAppContext->oLicStoreContextEmbedded,
                         &pAppContext->oDatastoreEST,
                         eDRM_LICENSE_STORE_XMR ) );

    fESTOpened = TRUE;

    dr = DRM_LST_GetLicense( poContextEMBEDDED, &oKid, &oLid, NULL, NULL, &cbLicense );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbLicense = ( DRM_BYTE * ) Oem_MemAlloc( cbLicense ) );
        ZEROMEM( pbLicense, cbLicense );
        ChkDR( DRM_LST_GetLicense( poContextEMBEDDED, &oKid, &oLid, NULL, pbLicense, &cbLicense ) );
    }
    else
    {
        ChkDR( dr );
    }
                
ErrorExit:
    if ( pAppContext != NULL )
    {
        if ( pAppContext->oLicStoreContextEmbedded.fInited )
        {
            DRM_LST_Close( &pAppContext->oLicStoreContextEmbedded );
        }
        if ( fESTOpened )
        {
            DRM_DST_CloseStore( &pAppContext->oDatastoreEST );
        }
    }    
    return dr;
}
#undef __cbMaxStack


/*****************************************************************************
** Function:    VerifyNumRemovalDateLicenses
**
** Synopsis:    Counts the number of licenses in the removal date store that have 
**              a removal date that is less than or equal to the specified removal 
**              date.
**
** Arguments:
**              argv[0] -- Removal date to check  - offset from the removal date base
**              argv[1] -- Expected number of licenses
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL VerifyNumRemovalDateLicenses( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                        = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pAppContext               = NULL;
    DRM_DWORD                 dwRemovalDate             = 0;
    DRMSYSTEMTIME            *pTestStartTime            = 0;
    DRM_WCHAR                 wszRemovalDate[64]        = {0};
    DRMFILETIME               ft                        = {0};
    DRM_DWORD                 cLicensesExpected         = 0;
    DRM_DWORD                 cLicensesActual           = 0;
    DRM_WCHAR                 wszRemovalDateOffset[256] = {0};
    DRM_DWORD                 cchRemovalDateOffset      = 0;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    GET_SHARED_APP_CONTEXT( pAppContext );

    /*
    ** The shared removal date variable is used as a common base time for the removal dates.
    ** The specified offset will be added to that common base time to create the removal
    ** date that will be used in the test.
    */
    cchRemovalDateOffset = DRMCRT_strlen( argv[0] ) + 1;
    ChkBOOL( cchRemovalDateOffset <= SIZEOF( wszRemovalDateOffset ) / SIZEOF( DRM_WCHAR ), DRM_E_BUFFERTOOSMALL );
    TST_UTL_mbstowcs( wszRemovalDateOffset, argv[0], cchRemovalDateOffset );

    MAP_TO_GLOBAL_TEST_VAR( pTestStartTime, LICENSE_REMOVAL_DATE_VAR );
    ChkDR(_CalcRelativeDate( wszRemovalDate, 
                             wszRemovalDateOffset, 
                             SIZEOF( wszRemovalDate ) / SIZEOF( DRM_WCHAR ), 
                             pTestStartTime ) );

    _TextTimeToFileTime(wszRemovalDate, &ft);
    tFileTimeToDWORD((DRMFILETIME*)&ft, &dwRemovalDate );

    /*
    ** Get the expected number of licenses
    */
    cLicensesExpected = strtol( argv[1], NULL, 10 );

    ChkDR( DRM_RDS_GetLicenseCount( &(pAppContext->oDatastoreHDS), 
                                    &dwRemovalDate,
                                    &cLicensesActual ) );

    if( cLicensesActual != cLicensesExpected )
    {
        Log( "Trace", "Number of Licenses (%d) in the RDS did not match the expected value", cLicensesActual );
        ChkDR( DRM_E_TEST_UNEXPECTED_LICENSE_COUNT );
    }

ErrorExit:

    return dr;
}


EXIT_PKTEST_NAMESPACE_CODE
