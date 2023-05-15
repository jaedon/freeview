/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <oemkeyfile.h>
#include <drmbcertparser.h>
#include <drmtoolsutils.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PK_NAMESPACE_CODE;
/* Overrideable hardware ID in OEM layer */
extern DRM_EXPORT_VAR DRM_BYTE g_rgbHWID[DRM_SHA1_DIGEST_LEN];
EXIT_PK_NAMESPACE_CODE;

ENTER_PKTEST_NAMESPACE_CODE

#define MAX_TEST_KEYFILE_KEYHISTORY 10

static const DRM_KEYHISTORY_ENTRY* s_rgoUpgradeTestKeyHistory[MAX_TEST_KEYFILE_KEYHISTORY] = { &g_oTestKeyfileKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static DRM_BYTE s_rgbKeyfileHash[DRM_SHA1_DIGEST_LEN] = {0};
static DRM_RESULT DRM_CALL _HashKeyfile(DRM_BOOL fCreate)
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL hKeyfile = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE *pbKeyfile = NULL;
    DRM_DWORD cbKeyfile = 0;
    DRM_DWORD cbRead = 0;
    DRM_SHA_CONTEXT oShaContext = {0};
    DRM_BYTE rgbCurrentHash[DRM_SHA1_DIGEST_LEN] = {0};

    hKeyfile = Oem_Keyfile_Open( NULL, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );
    ChkFAIL( hKeyfile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_GetSize( hKeyfile, &cbKeyfile ), DRM_E_FILE_READ_ERROR );
    ChkFAIL( cbKeyfile <= DRM_KEYFILE_MAX_SIZE );

    ChkMem(pbKeyfile = (DRM_BYTE*)Oem_MemAlloc( cbKeyfile ));
    ChkBOOL( Oem_File_SetFilePointer( hKeyfile, 0, OEM_FILE_BEGIN, NULL ), DRM_E_FILE_SEEK_ERROR );
    ChkBOOL( Oem_File_Read( hKeyfile, pbKeyfile, cbKeyfile, &cbRead ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( cbRead == cbKeyfile, DRM_E_FILE_READ_ERROR );

    ChkDR( DRM_SHA_Init( &oShaContext, eDRM_SHA_1 ) );

    ChkDR( DRM_SHA_Update( pbKeyfile, cbKeyfile, &oShaContext ) );

    ChkDR( DRM_SHA_Finalize( &oShaContext, DRM_SHA1_DIGEST_LEN, rgbCurrentHash ) );

    if( fCreate )
    {
        MEMCPY( s_rgbKeyfileHash, rgbCurrentHash, DRM_SHA1_DIGEST_LEN );
    }
    else
    {
        ChkFAIL( MEMCMP( s_rgbKeyfileHash, rgbCurrentHash, DRM_SHA1_DIGEST_LEN ) == 0 );
    }

ErrorExit:
    if( hKeyfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_Keyfile_Close( hKeyfile );
    }
    SAFE_OEM_FREE( pbKeyfile );
    return dr;

}


/*****************************************************************************
** Function:    TestManagerHashKeyfile
**
** Synopsis:    Creates or verifies a hash of the keyfile
**
** Arguments:   argv[0] : CREATE_HASH to create a hash, VERIFY_HASH to verify the hash
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerHashKeyfile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    if( 0 == strncmp( argv[0], "CREATE_HASH", 11 ) )
    {
        ChkDR( _HashKeyfile( TRUE ) );
    }
    else if( 0 == strncmp( argv[0], "VERIFY_HASH", 11 ) )
    {
        ChkDR( _HashKeyfile( FALSE ) );
    }
    else
    {
        ChkArg( FALSE );
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerSetRobustnessVersion
**
** Synopsis:    Sets the firmware's reported robustness version
**
** Arguments:   argv[0] : Robustness version to set
**              argv[1] : Platform ID to set in decimal
**              argv[2] : If present then it specifies WMDRM or PR robustness version:
**                        1 for WMDRM, 2 for PR.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetRobustnessVersion( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrSecurityVersion  = EMPTY_DRM_STRING;
    DRM_DWORD dwIndex = 0;

    ChkArg( argc == 2 || argc== 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrSecurityVersion, argv[0] ) );
    if ( argc == 2 )
    {
        ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                    dstrSecurityVersion.cchString,
                                                    &g_dwWMRobustnessVersion ) );
        ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                    dstrSecurityVersion.cchString,
                                                    &g_dwPRRobustnessVersion ) );
    }
    else
    {
        /* set only PlayReady or WM robustness version */
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[2]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[2], cch, 10, &dwIndex ) );
        switch ( dwIndex )
        {
            case 1:
                ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                            dstrSecurityVersion.cchString,
                                                            &g_dwWMRobustnessVersion ) );
                break;
            case 2:
                ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                            dstrSecurityVersion.cchString,
                                                            &g_dwPRRobustnessVersion ) );
                break;
            default:
                ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    g_dwPlatformID = (DRM_DWORD)atoi( argv[1] );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetHardwareID
**
** Synopsis:    Sets the firmware's hardware ID
**
** Arguments:   argv[0] : Base64 hardware ID to set
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetHardwareID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dsstr = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cbDest = SIZEOF( g_rgbHWID );

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    dsstr.m_ich = 0;
    dsstr.m_cch = (DRM_DWORD)strlen(argv[0]);
    ChkDR( DRM_B64_DecodeA( argv[0], &dsstr, &cbDest, g_rgbHWID, 0 ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerValidateRobustnessVersion
**
** Synopsis:    Validates the firmware's reported robustness version
**
** Arguments:   argv[0] : Robustness version to validate
**              argv[1] : Tells which robustness version to validate: WM (1) or PR (2)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerValidateRobustnessVersion( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwVersion = 0;
    DRM_DWORD  dwOutVersion = 0;
    DRM_DWORD  dwOutPlatform = 0;
    DRM_BYTE  *pbDevCert = NULL;
    DRM_DWORD  cbDevCert = 0;
    DRM_CONST_STRING dstrSecurityVersion  = EMPTY_DRM_STRING;
    DRM_DWORD dwIndex = 0;
    DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDevCertSecurityVersion = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT  *pcryptoCtx = NULL;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkMem( pcryptoCtx = (DRM_CRYPTO_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT) ) );
    ZEROMEM( pcryptoCtx, SIZEOF(DRM_CRYPTO_CONTEXT) );

    /*
    ** Robustness version value to compare against
    */
    ChkDR( MakeDRMString( &dstrSecurityVersion, argv[0] ) );
    ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrSecurityVersion.pwszString,
                                                dstrSecurityVersion.cchString,
                                                &dwVersion ) );

    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &dwIndex ) );
    }

    switch ( dwIndex )
    {
        case 1:
            ChkDR( Oem_Keyfile_GetRobustnessVersion( NULL,
                                                     KF_CERT_TYPE_WMDRM,
                                                     &dwOutVersion,
                                                     &dwOutPlatform ) );
            ChkFAIL( dwVersion == dwOutVersion );

            ChkDR( TST_UTL_GetWMDRMCert( (DRM_STRING*)&dstrDevCert, pcryptoCtx ) );
            dr = DRM_DCP_GetAttribute( NULL,
                                       &dstrDevCert,
                                       DRM_DEVCERT_GROUPSECURITYVERSIONNUM,
                                       NULL,
                                       &dstrDevCertSecurityVersion );
            if ( dr == DRM_E_XMLNOTFOUND )
            {
                dwOutVersion = 0; /* if security version is not present then test should think it's 0 */
            }
            else
            {
                ChkDR( DRM_UTL_GetVersionFromStringAsDWORD( dstrDevCertSecurityVersion.pwszString,
                                                            dstrDevCertSecurityVersion.cchString,
                                                            &dwOutVersion ) );
            }
            ChkFAIL( dwVersion == dwOutVersion );

            break;
        case 2:
            ChkDR( Oem_Keyfile_GetRobustnessVersion( NULL,
                                                     KF_CERT_TYPE_PLAYREADY,
                                                     &dwOutVersion,
                                                     &dwOutPlatform ) );
            ChkFAIL( dwVersion == dwOutVersion );


            ChkMem( pbDevCert = (DRM_BYTE*) Oem_MemAlloc(MAX_DEVICE_CERT_SIZE) );
            cbDevCert = MAX_DEVICE_CERT_SIZE;

            dwOutVersion = 0;
            dwOutPlatform = 0;
            ChkDR( TST_UTL_GetPlayReadyCert(pbDevCert, &cbDevCert, pcryptoCtx) );
            ChkDR( DRM_BCert_GetSecurityVersion( pbDevCert,
                                                 cbDevCert,
                                                &dwOutVersion,
                                                &dwOutPlatform ) );
            ChkFAIL( dwVersion == dwOutVersion );

            break;
        default:
            ChkDR( DRM_E_TEST_INVALIDARG );
    }


ErrorExit:
    SAFE_OEM_FREE( pcryptoCtx );
    return dr;
}


/*****************************************************************************
** Function:    TestManagerAddNewKeyfileKey
**
** Synopsis:    Adds a new random keyfile decryption key to the OEM layer and sets it as the "current" key.
**              If argv[0] is present then load a key from the file instead.
**
** Arguments:   argv[0] : optional, file that contains AES key
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddNewKeyfileKey( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwSize = 0;

    ChkArg( argc <= 1 );
    ChkArg( g_dwKeyHistoryCount < MAX_TEST_KEYFILE_KEYHISTORY );

    if( argc > 0 && argv[0] != NULL )
    {
        /* Load from file */
        LoadTestFile( NULL, argv[0], (DRM_BYTE**)&(s_rgoUpgradeTestKeyHistory[g_dwKeyHistoryCount]), &dwSize );
        ChkBOOL( dwSize == SIZEOF( DRM_KEYHISTORY_ENTRY ), DRM_E_TEST_INVALID_FILE );
    }
    else
    {
        /* Allocate memory for the new key (freed by TestManagerFreeKeyfileKeys) */
        ChkMem( s_rgoUpgradeTestKeyHistory[g_dwKeyHistoryCount] = (DRM_KEYHISTORY_ENTRY*)Oem_MemAlloc( SIZEOF( DRM_KEYHISTORY_ENTRY ) ) );

        /* Generate random key and ID */
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)s_rgoUpgradeTestKeyHistory[g_dwKeyHistoryCount], SIZEOF( DRM_KEYHISTORY_ENTRY ) ) );
    }

    /* Increment key count as we've just added a new key */
    g_ppKeyHistoryList = (DRM_KEYHISTORY_ENTRY **)&s_rgoUpgradeTestKeyHistory;
    g_dwKeyHistoryCount++;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerFreeKeyfileKeys
**
** Synopsis:    Frees keys added by TestManagerAddNewKeyfileKey, leaves the static original keyfile key
**
** Arguments:   argv[0] : Number of keys to free, starting from the most current. If NULL, frees all added keys
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerFreeKeyfileKeys( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwKeysToFree = 0;
    DRM_DWORD  i = 0;

    ChkArg( argc >= 0 );

    ChkArg( g_dwKeyHistoryCount <= MAX_TEST_KEYFILE_KEYHISTORY );

    if( argc > 0
     && argv[0] != NULL )
    {
        dwKeysToFree = (DRM_DWORD)atoi( argv[0] );
        ChkArg( dwKeysToFree < g_dwKeyHistoryCount );
    }
    else
    {
        if( g_dwKeyHistoryCount <= 1 )
        {
            /* We never free the last key, it's a static local */
            goto ErrorExit;
        }
        dwKeysToFree = g_dwKeyHistoryCount - 1;
    }

    g_dwKeyHistoryCount -= dwKeysToFree;

    /* Free all but the first key, the first key is a static variable */
    for( i = 1; i <= dwKeysToFree; i++ )
    {
        SAFE_OEM_FREE( s_rgoUpgradeTestKeyHistory[i] );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

