/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmopcodes.h>
#include <drmmanagerconstants.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

const DRM_WCHAR wDLRB[]  = { ONE_WCHAR('d','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('b','\0'), ONE_WCHAR('\0','\0') } ;

/*****************************************************************************
** Function:    _GetNextKID
**
** Synopsis:
**
** Arguments:   pdstrKID :
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
static DRM_RESULT _GetNextKID( DRM_CONST_STRING *pdstrKID )
{
    DRM_RESULT   dr  = DRM_SUCCESS;
    DRM_KID      kid = {0};
    DRM_DWORD   *pdw = NULL;

    ChkDR( DRM_UTL_DecodeKID( pdstrKID, &kid ) );
    pdw = (DRM_DWORD*)&kid;
    (*pdw)++;
    ChkDR( DRM_B64_EncodeW( (DRM_BYTE*)&kid,
                            SIZEOF(kid),
                            (DRM_WCHAR*)pdstrKID->pwszString,
                            &pdstrKID->cchString,
                            0 ) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _AddKID
**
** Synopsis:
**
** Arguments:   pbXML :
**              szStartingKID :
**              cKID :
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
static DRM_RESULT _AddKID(
                 _XMBContext *pbXML,
    __in_z const char        *szStartingKID,
                 int          cKID )
{
    DRM_RESULT          dr      = DRM_SUCCESS;
    DRM_LONG            i       = 0;
    DRM_CONST_STRING    dstrKID = EMPTY_DRM_STRING;

    ChkDR( MakeDRMString( &dstrKID, szStartingKID ) );

    for (i = 0; i < cKID; i++)
    {
        ChkDR( DRM_XMB_OpenNode( pbXML, &g_dstrTagKID ) );
        ChkDR( DRM_XMB_AddAttribute( pbXML, &g_dstrLabelValue, &dstrKID ) );
        ChkDR( DRM_XMB_CloseCurrNode( pbXML, NULL ) );

        ChkDR( _GetNextKID( &dstrKID ) );
    }
ErrorExit:
    FREEDRMSTRING( dstrKID );
    return dr;
}


/*****************************************************************************
** Function:    TestManagerMakeDeviceLicenseRevocationBlob
**
** Synopsis:    Generate a DLRB
**
** Arguments:   argv[0]: LGPUBKEY
**              argv[1]: LG signing private key
**              argv[2]: starting KID string
**              argv[3]: number of KIDs in the DLRB
**              argv[4]: optional flags to DLRB generation
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerMakeDeviceLicenseRevocationBlob(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr              = DRM_SUCCESS;
    const DRM_CONST_STRING  dstrDLRB        = CREATE_DRM_STRING( wDLRB );
    DRM_CONST_STRING        dstrPubKey      = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrSignature   = EMPTY_DRM_STRING;
    DRM_STRING              dstrDataBlock   = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT     *pcrypto         = NULL;
    DRM_SUBSTRING           subStr          = {0};
    PRIVKEY                 prvKey          = {0};
    DRM_DWORD               cb              = 0;
    DRM_DWORD              *pcbDLRB         = NULL;
    DRM_STRING             *pdstrDLRB       = NULL;
    _XMBContext            *pbDLRB          = NULL;
    DRM_WCHAR               wszB64Sign[DRM_ECC160_SIGNATURE_B64LEN] = {0};

    ChkArg( argc >= 4 );
    ChkArg( argv[3] );

    ChkMem( pcrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    /*
    ** Get shared variables
    */
    MAP_TO_GLOBAL_TEST_VAR( pdstrDLRB, DEVICE_LICENSE_REV_BLOB_STRING_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcbDLRB, DEVICE_LICENSE_REV_BLOB_SIZE_VAR );
    if ( DRM_FAILED( DRM_TST_GetTestVar( DEVICE_LICENSE_REV_BLOB_VAR, ( DRM_VOID ** ) &pbDLRB, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbDLRB, DEVICE_LICENSE_REV_BLOB_VAR, *pcbDLRB );
    }

    ChkDR( DRM_XMB_CreateDocument( *pcbDLRB, pbDLRB, &g_dstrTagDeviceLicenseRevocationRoot ) );
    ChkDR( DRM_XMB_AddAttribute( pbDLRB, &g_dstrAttributeType, &dstrDLRB ) );

    /*
    ** Add Data Node
    */
    ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagData ) );

        /*
        ** Add LGPUBKEY
        */
        ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagRevocationPubKey ) );
        ChkDR( MakeDRMString( &dstrPubKey, argv[0] ) );
        ChkDR( DRM_XMB_AddAttribute( pbDLRB, &g_dstrLabelValue, &dstrPubKey ) );
        ChkDR( DRM_XMB_CloseCurrNode( pbDLRB, &dstrDataBlock ) );

        ChkDR( _AddKID( pbDLRB, argv[2], atol( argv[3] ) ) );

    /*
    ** Get the signing private key
    */
    subStr.m_ich = 0;
    subStr.m_cch = ( DRM_DWORD ) strlen( argv[1] );
    cb = sizeof(PRIVKEY);
    ChkDR( DRM_B64_DecodeA( argv[1], &subStr, &cb, (DRM_BYTE*)&prvKey, 0 ) );

    if ( argc > 4 && !strcmp( argv[4], "BAD_SIGNATURE" ) )
    {
        prvKey.x[0]++;
    }

    ChkDR( DRM_XMB_SignAndCloseCurrNode( pbDLRB, pcrypto, &prvKey, TRUE, NULL, wszB64Sign, NULL ) );

    /*
    ** Add SIGNATURE
    */
    ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagSignature ) );
        ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagHashAlg ) );
        ChkDR( DRM_XMB_AddAttribute( pbDLRB, &g_dstrAttributeType, &g_dstrSHA ) );
        ChkDR( DRM_XMB_CloseCurrNode( pbDLRB, NULL ) );

        ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagSignAlg ) );
        ChkDR( DRM_XMB_AddAttribute( pbDLRB, &g_dstrAttributeType, &g_dstrMSDRM ) );
        ChkDR( DRM_XMB_CloseCurrNode( pbDLRB, NULL ) );

        DSTR_FROM_PB( &dstrSignature, wszB64Sign, SIZEOF( wszB64Sign ) );
        ChkDR( DRM_XMB_OpenNode( pbDLRB, &g_dstrTagValue ) );
        ChkDR( DRM_XMB_AddData( pbDLRB, &dstrSignature ) );
        ChkDR( DRM_XMB_CloseCurrNode( pbDLRB, NULL ) );
    ChkDR( DRM_XMB_CloseCurrNode( pbDLRB, NULL ) );

    ChkDR( DRM_XMB_CloseDocument( pbDLRB, pdstrDLRB ) );
ErrorExit:

    SAFE_OEM_FREE( pcrypto );
    FREEDRMSTRING( dstrPubKey );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerProcessDeviceLicenseRevocation
**
** Synopsis:    Generate a DLRB
**
** Arguments:   argv[0]: index to the starting KID
**              argv[1]: number of KIDs to revoke
**              argv[2]: status of DLRB: NORMAL or NULL
**              argv[3]: expected number of KIDs processed
**              argv[4]: expected number of licenses deleted
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessDeviceLicenseRevocation(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_DWORD        dwcKidsProcessed = 0;
    DRM_DWORD        dwcLicsDeleted   = 0;
    DRM_STRING      *pdstrDLRB        = NULL;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( pdstrDLRB, DEVICE_LICENSE_REV_BLOB_STRING_VAR );

    ChkArg( argc == 5 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL );

    ChkDR( Drm_ProcessCommand( argv[2]? poManagerContext: NULL,
                               OpProcessLicenseRevocationList,
                               (DRM_DWORD)atol( argv[0] ),
                               (DRM_DWORD)atol( argv[1] ),
                               0,
                               0,
                               (DRM_BYTE*)pdstrDLRB->pwszString,
                               pdstrDLRB->cchString * SIZEOF( DRM_WCHAR ),
                               ( argv[3] && strcmp( argv[3], "IGNORE" ) )? &dwcKidsProcessed: NULL,
                               ( argv[4] && strcmp( argv[4], "IGNORE" ) )? &dwcLicsDeleted: NULL,
                               NULL,
                               NULL ) );

    /*
    ** Check return values
    */
    if ( argv[3] && atol( argv[3] ) != (DRM_LONG) dwcKidsProcessed )
    {
        Log( "Trace", "Number of processed KIDs does not match. Returning = %d. Expecting %d.", dwcKidsProcessed, atol( argv[3] ) );
        ChkDR( DRM_E_FAIL );
    }

    if ( argv[4] && atol( argv[4] ) != (DRM_LONG) dwcLicsDeleted )
    {
        Log( "Trace", "Number of deleted licenses does not match. Returning = %d. Expecting %d.", dwcLicsDeleted, atol( argv[4] ) );
        ChkDR( DRM_E_FAIL );
    }
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */
