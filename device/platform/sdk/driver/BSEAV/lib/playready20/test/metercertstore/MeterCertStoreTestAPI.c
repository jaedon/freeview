/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <MeterCertStoreTestAPI.h>
#include <drmdevicestore.h>
#include <drmmetercertstore.h>
#include <drmmetering.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    TestMeterCertUpdate
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to update
**              argv[1] -- new MeterCert Value
**              argv[2] -- new LAINFO Value
**              argv[3] -- opt, if NULL use a null MeterCertContext
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestMeterCertUpdate( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_SUBSTRING            dsstrTemp          = EMPTY_DRM_SUBSTRING;
    DRM_MID                  mid                = {0};
    DRM_MID                 *pMid               = NULL;
    DRM_DWORD                cbmid              = SIZEOF( mid.rgb );
    DRM_STRING               dstrMeterCert      = EMPTY_DRM_STRING;
    DRM_STRING               dstrLAINFO         = EMPTY_DRM_STRING;
    DRM_CONST_STRING        *pdcstrLAINFO       = NULL;
    DRM_CONST_STRING        *pdcstrMeterCert    = NULL;
    DRM_METERCERT_CONTEXT   *pMeterCertContext  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( argc >= 3 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[2] != NULL )
    {
        dstrLAINFO.cchString  = ( DRM_DWORD ) ( strlen( argv[2] ) + 1 ) / SIZEOF( DRM_WCHAR );
        dstrLAINFO.pwszString = ( DRM_WCHAR * ) argv[2];
        pdcstrLAINFO = ( DRM_CONST_STRING * ) &dstrLAINFO;
    }
    if ( argv[1] != NULL )
    {
        dstrMeterCert.cchString  = ( DRM_DWORD ) ( strlen( argv[1] ) + 1 ) / SIZEOF( DRM_WCHAR );
        dstrMeterCert.pwszString = ( DRM_WCHAR * ) argv[1];
        pdcstrMeterCert = ( DRM_CONST_STRING * ) &dstrMeterCert;
    }
    if ( argv[0] != NULL )
    {
        dsstrTemp.m_ich = 0;
        dsstrTemp.m_cch = ( DRM_DWORD ) strlen( argv[0] );

        ChkDR( DRM_B64_DecodeA( argv[0], &dsstrTemp, &cbmid, mid.rgb, 0 ) );
        pMid = &mid;
    }
    ChkDR( DRM_MCS_UpdateMeterCert( ( argc > 3 && argv[3] == NULL ) ? NULL : pMeterCertContext,
                                    pdcstrLAINFO,
                                    TRUE, /* Only V2 certs are supported by this test API */
                                    pdcstrMeterCert != NULL ? PB_DSTR( pdcstrMeterCert ) : NULL,
                                    pdcstrMeterCert != NULL ? CB_DSTR( pdcstrMeterCert ) : 0,
                                    pMid ) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestMeterCertGetByMID
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to get
**              argv[1] -- The MeterCert expected
**              argv[2] -- The LAINFO string expected
**              argv[3] -- opt, set metercert cch too small on second call if TRUE.
**              argv[4] -- opt, set lainfo cch too small on second call if TRUE.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestMeterCertGetByMID( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT      dr            = DRM_SUCCESS;
    DRM_SUBSTRING   dsstrMID      = EMPTY_DRM_SUBSTRING;
    DRM_MID         mid           = {0};
    DRM_MID        *pMid          = NULL;
    DRM_DWORD       cbmid         = SIZEOF( mid.rgb );
    DRM_WCHAR      *pwszMeterCert = NULL;
    DRM_WCHAR      *pwszLAINFO    = NULL;
    DRM_DWORD       cchMeterCert  = 1;
    DRM_DWORD       cbMeterCert   = 0;
    DRM_DWORD       cchLAINFO     = 1;
    DRM_BOOL       fGoodCchLA    = TRUE;
    DRM_BOOL       fGoodCchCert  = TRUE;
    DRM_METERCERT_CONTEXT   *pMeterCertContext  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( argc >= 3 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[0] != NULL )
    {
        dsstrMID.m_ich = 0;
        dsstrMID.m_cch = ( DRM_DWORD ) strlen( argv[0] );

        ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbmid, mid.rgb, 0 ) );
        pMid = &mid;
    }

    if ( argc >= 4 )
    {
        ChkArg( DRM_SUCCEEDED( StringToBool( argv[3], &fGoodCchCert ) ) );
    }

    if ( argc >= 5 )
    {
        ChkArg( DRM_SUCCEEDED( StringToBool( argv[4], &fGoodCchLA ) ) );
    }


    dr = DRM_MCS_GetMeterCertByMID( pMeterCertContext,
                                    pMid,
                                    NULL,
                                   &cchLAINFO,
                                    NULL,
                                    NULL,
                                   &cchMeterCert );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
        ChkBOOL( dr != DRM_SUCCESS, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }


    if ( !fGoodCchLA )
    {
        cchLAINFO -= 1;
    }

    if ( !fGoodCchCert )
    {
        cchMeterCert -= 1;
    }

    ChkMem( pwszMeterCert = ( DRM_WCHAR * ) Oem_MemAlloc( max(1, cchMeterCert * SIZEOF( DRM_WCHAR )) ) );
    ChkMem( pwszLAINFO    = ( DRM_WCHAR * ) Oem_MemAlloc( max(1, cchLAINFO * SIZEOF( DRM_WCHAR )) ) );

    cbMeterCert = cchMeterCert * SIZEOF( DRM_WCHAR );

    dr = DRM_MCS_GetMeterCertByMID( pMeterCertContext,
                                   &mid,
                                    pwszLAINFO,
                                   &cchLAINFO,
                                    NULL,
                                    ( DRM_BYTE * )pwszMeterCert,
                                   &cbMeterCert );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    dr = DRM_SUCCESS;

    cchMeterCert = cbMeterCert / SIZEOF( DRM_WCHAR );

    ChkBOOL( ( ( DRM_DWORD ) ( strlen( argv[2] ) + 1 ) / SIZEOF( DRM_WCHAR ) ) == cchLAINFO, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !fGoodCchLA || ( DRMCRT_wcsncmp( pwszLAINFO, ( DRM_WCHAR * ) argv[2], cchLAINFO ) == 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( ( ( DRM_DWORD ) ( strlen( argv[1] ) + 1) / SIZEOF( DRM_WCHAR ) ) == cchMeterCert, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !fGoodCchCert || ( DRMCRT_wcsncmp( pwszMeterCert, ( DRM_WCHAR * ) argv[1], cchMeterCert ) == 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pwszMeterCert );
    SAFE_OEM_FREE( pwszLAINFO );
    return dr;
}

/*****************************************************************************
** Function:    TestDeleteMeterCert
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to delete, can be NULL
**              argv[1] -- opt, if NULL a null pointer is used for the context pointer
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestDeleteMeterCert( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT    dr       = DRM_SUCCESS;
    DRM_SUBSTRING dsstrMID = EMPTY_DRM_SUBSTRING;
    DRM_MID       mid      = {0};
    DRM_MID      *pMid     = NULL;
    DRM_DWORD     cbmid    = SIZEOF( mid.rgb );
    DRM_METERCERT_CONTEXT   *pMeterCertContext  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( argc >= 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[0] != NULL )
    {
        dsstrMID.m_ich = 0;
        dsstrMID.m_cch = ( DRM_DWORD ) strlen( argv[0] );

        ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbmid, mid.rgb, 0 ) );
        pMid = &mid;
    }
    ChkDR( DRM_MCS_DeleteMeterCert( ( argc >= 2 && argv[1] == NULL ) ? NULL : pMeterCertContext,
                                    pMid ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestInvalidateMeterCert
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to invalidate, can be NULL
**              argv[1] -- opt, if NULL a null pointer is used for the context pointer
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestInvalidateMeterCert( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_SUBSTRING            dsstrMID           = EMPTY_DRM_SUBSTRING;
    DRM_MID                  mid                = {0};
    DRM_MID                 *pMid               = NULL;
    DRM_DWORD                cbmid              = SIZEOF( mid.rgb );
    DRM_METERCERT_CONTEXT   *pMeterCertContext  = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( argc >= 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[0] != NULL )
    {
        dsstrMID.m_ich = 0;
        dsstrMID.m_cch = ( DRM_DWORD ) strlen( argv[0] );

        ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbmid, mid.rgb, 0 ) );
        pMid = &mid;
    }
    ChkDR( DRM_MCS_InvalidateMeterCert( ( argc >= 2 && argv[1] == NULL ) ? NULL : pMeterCertContext,
                                        pMid ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestOpenMeterCertStore
**
** Synopsis:
**
** Arguments:
**              argv[0] != NULL then use a valid dst context
**              argv[1] != NULL then use a valid pbBuffer
**
**              All are assumed != NULL if argc == 0
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestOpenMeterCertStore( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_BOOL                 fValidDST          = TRUE;
    DRM_BOOL                 fValidpbBuffer     = TRUE;
    DRM_APP_CONTEXT         *poManagerContext   = NULL;
    DRM_METERCERT_CONTEXT   *pMeterCertContext  = NULL;
    DRM_BYTE                *pbBuffer           = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( pbBuffer, METER_CERT_BUFFER_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( 2 >= argc );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    fValidDST      = ( 1 <= argc && argv[0] == NULL ) ? FALSE : TRUE;
    fValidpbBuffer = ( 2 <= argc && argv[1] == NULL ) ? FALSE : TRUE;

    ChkDR( DRM_MCS_OpenStore( fValidDST ? &( ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext )->oDatastoreHDS : NULL,
                              fValidpbBuffer ? pbBuffer : NULL,
                              fValidpbBuffer ? METERCERT_BUFFER_SIZE : 0 ,
                              pMeterCertContext ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestCloseMeterCertStore
**
** Synopsis:
**
** Arguments:
**              argv[0] == NULL then use a NULL pointer for pMeterCertContext
**              else use g_pMeterCertContext
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestCloseMeterCertStore( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_METERCERT_CONTEXT   *pMeterCertContext     = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );

    ChkArg( 1 >= argc );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( DRM_MCS_CloseStore( ( 1 <= argc && NULL == argv[0] ) ? NULL : pMeterCertContext ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestMeterCertInitEnum
**
** Synopsis:
**
** Arguments:
**              argv[0] != NULL, use g_pMeterCertContext
**              argv[1] != NULL, use a valid pointer for the Enum structure
**
**              no args, both are assumed !NULL
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestMeterCertInitEnum( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_METERCERT_CONTEXT   *pMeterCertContext     = NULL;
    DRM_METERCERT_ENUM      *pcontextMeterCertEnum = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcontextMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR );

    ChkArg( 2 >= argc );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( DRM_MCS_InitEnum( ( 1 <= argc && argv[0] == NULL ) ? NULL : pMeterCertContext,
                             ( 2 <= argc && argv[1] == NULL ) ? NULL : pcontextMeterCertEnum) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestMeterCertEnumNext
**
** Synopsis:
**
** Arguments:
**              argv[0] -- mid
**              argv[1] -- lainfo
**              argv[2] -- opt, if NULL use a null MeterCertEnum structure
**              argv[3] -- opt, metercert -- doesn't have to be a valid cert since the DRM_MGR update function validates, not this one.
**              argv[4] -- opt, if NULL use a NULL for the slot hint argument
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestMeterCertEnumNext( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_SUBSTRING   dsstrMID        = EMPTY_DRM_SUBSTRING;
    DRM_MID         mid             = {0};
    DRM_DWORD       cbMID           = SIZEOF( mid.rgb );
    DRM_MID         midReturned     = {0};
    DRM_WCHAR       rgwcLAINFO[100] = {0};
    DRM_DWORD       cchLAINFO       = NO_OF( rgwcLAINFO );
    DRM_WCHAR      *pwszMeterCert   = NULL;
    DRM_DWORD       cchMeterCert    = DRM_MAX_TEST_METERCERT_SIZE;
    DRM_DWORD       cbMeterCert     = 0;
    DRM_DST_SLOT_HINT slotHint      = {0};
    DRM_METERCERT_CONTEXT   *pMeterCertContext     = NULL;
    DRM_METERCERT_ENUM      *pcontextMeterCertEnum = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcontextMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR );

    ChkArg( argc >= 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

#pragma prefast(push)
#pragma prefast(disable:22011) /* already checking integer overflow */
    ChkOverflow( cchMeterCert * SIZEOF( DRM_WCHAR ), cchMeterCert );
    ChkMem( pwszMeterCert = ( DRM_WCHAR * )Oem_MemAlloc( cchMeterCert * SIZEOF( DRM_WCHAR ) ) );
#pragma prefast(pop)

    if( argv[0] != NULL )
    {
        dsstrMID.m_cch = ( DRM_DWORD ) strlen( argv[0] );
        dsstrMID.m_ich = 0;
        if ( dsstrMID.m_cch > 0 )
        {
            ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbMID, mid.rgb, 0 ) );
        }
    }
    ChkDR( DRM_MCS_EnumNext( ( 3 <= argc && argv[2] == NULL ) ? NULL :  pcontextMeterCertEnum,
                             ( argv[0] == NULL )              ? NULL : &midReturned,
                             ( 5 <= argc && argv[4] == NULL ) ? NULL : &slotHint ) );

    if( argv[0] != NULL )
    {
        ChkArg( argv[1] != NULL );

        cbMeterCert = cchMeterCert * SIZEOF( DRM_WCHAR );

        ChkDR( DRM_MCS_GetMeterCertByMID( pMeterCertContext,
                                         &midReturned,
                                          rgwcLAINFO,
                                         &cchLAINFO,
                                          NULL,
                                          ( DRM_BYTE * )pwszMeterCert,
                                         &cbMeterCert ) );

        cchMeterCert = cbMeterCert / SIZEOF( DRM_WCHAR );

        ChkBOOL( strncmp( (const DRM_CHAR *)midReturned.rgb, (const DRM_CHAR *)mid.rgb, cbMID ) == 0 , DRM_E_FAIL );
        ChkBOOL( DRMCRT_wcsncmp( ( DRM_WCHAR * ) argv[1], rgwcLAINFO, cchLAINFO ) == 0, DRM_E_FAIL );
    }

    if ( argc >= 4 )
    {
        ChkArg( argv[3] != NULL );
        ChkBOOL( DRMCRT_wcsncmp( ( DRM_WCHAR * )argv[3], pwszMeterCert, cchMeterCert ) == 0, DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE( pwszMeterCert );
    return dr;
}


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(MeterCertStore)( long lTCID, const char *strTCName )
{
    DRM_RESULT               dr                    = DRM_SUCCESS;

    if( DRM_METERING_IsMeteringSupported() )
    {
        DRM_METERCERT_CONTEXT   *pMeterCertContext     = NULL;
        DRM_BYTE                *pbBuffer              = NULL;
        DRM_METERCERT_ENUM      *pcontextMeterCertEnum = NULL;

        /*
        ** Set shared metercert variables
        */
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbBuffer, METER_CERT_BUFFER_VAR, METERCERT_BUFFER_SIZE );

        MAP_TO_GLOBAL_TEST_VAR_Alloc( pMeterCertContext, DRM_METERCERT_CONTEXT_VAR, SIZEOF( DRM_METERCERT_CONTEXT ) );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pcontextMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR, SIZEOF( DRM_METERCERT_ENUM ) );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(MeterCertStore)( long lTCID, const char *strTCName )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;

    return dr;
}

BEGIN_TEST_API_LIST(MeterCertStore)
    API_ENTRY(TestMeterCertUpdate)
    API_ENTRY(TestMeterCertGetByMID)
    API_ENTRY(TestDeleteMeterCert)
    API_ENTRY(TestInvalidateMeterCert)
    API_ENTRY(TestOpenMeterCertStore)
    API_ENTRY(TestCloseMeterCertStore)
    API_ENTRY(TestMeterCertInitEnum)
    API_ENTRY(TestMeterCertEnumNext)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
