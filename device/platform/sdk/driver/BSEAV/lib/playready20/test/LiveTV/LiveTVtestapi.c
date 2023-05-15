/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <LiveTVTestAPI.h>
#include <DrmManagerTestAPI.h>
#include <trightsparser.h>
#include <headergen.h>
#include <drmutf.h>
#include "drmheaderparser.h"
#include "drmplayreadyobj.h"

USING_PKTEST_NAMESPACE

ENTER_PKTEST_NAMESPACE_CODE

/*********************************************************************
**
** Function: TestVerifyStoreSlotCount
**
** Synopsis: Verifies slot count in a store
**
** Arguments:
**
** [argc]    - Number of arguments
** [argv[0]] - Name of the store
** [argv[1]] - The expected number of slots.
** [argv[2]] - Path the store file. optional - HDS is the default.
**
** Returns:  - a valid return code
**
******************************************************************************/
DRM_RESULT DRM_CALL TestVerifyStoreSlotCount( __in DRM_LONG argc, __in_ecount( argc ) DRM_CHAR** argv )
{
    DRM_RESULT       dr                      = DRM_SUCCESS;
    DRM_DWORD        dwExpected              = 0;
    DRM_DWORD        dwActual                = 0;
    DRM_CONST_STRING dstrFileName            = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNoDeleteFileName    = EMPTY_DRM_STRING;
    const DRM_WCHAR *pwszFileName            = NULL;
    DRM_WCHAR        wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};
    
    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &dwExpected ) );

    if( argc >= 3 && argv[2] != NULL  )
    {
        ChkDR( MakeDRMString( &dstrFileName, argv[2] ) );
        ChkMem( dstrFileName.pwszString );
        pwszFileName = dstrFileName.pwszString;
    }
    else
    {
        ChkDR( tGetDeviceStorePathname( wszPathName, &dstrNoDeleteFileName ) );
        pwszFileName = dstrNoDeleteFileName.pwszString;
    }

    ChkDR( TST_GetNameSpaceSlotCount( pwszFileName, argv[0], &dwActual ) );

    Log( "Test_VerifyStoreSlotCount", "Expected slot count = %d, Actual slot count = %d", dwExpected, dwActual );
    ChkBOOL( dwExpected == dwActual, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    return dr;
}

/*********************************************************************
**
** Function: TestVerifySecureStoreEntry
**
** Synopsis: Verifies entry in a secure store
**
** Arguments:
**
** [argc]    - Number of arguments
** [argv[0]] - LID
** [argv[1]] - Path the store file. optional - HDS is the default.
**
** Returns:  - DRM_SUCCESS if there was an entry.  DRM_S_FALSE otherwise
**
******************************************************************************/
DRM_RESULT DRM_CALL TestVerifySecureStoreEntry( __in DRM_LONG argc, __in_ecount( argc ) DRM_CHAR** argv )
{
    DRM_RESULT       dr                                         = DRM_SUCCESS;
    DRM_CONST_STRING dstrFileName                               = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNoDeleteFileName                       = EMPTY_DRM_STRING;
    const DRM_WCHAR *pwszFileName                               = NULL;
    DRM_WCHAR        wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};
	DRM_ID           idLID                                      = {0};
    DRM_SUBSTRING    dsstrTemp                                  = EMPTY_DRM_SUBSTRING;
    DRM_DWORD        cbLID                                      = SIZEOF( DRM_ID );

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    if( argc >= 2 && argv[1] != NULL  )
    {
        ChkDR( MakeDRMString( &dstrFileName, argv[1] ) );
        ChkMem( dstrFileName.pwszString );
        pwszFileName = dstrFileName.pwszString;
    }
    else
    {
        ChkDR( tGetDeviceStorePathname( wszPathName, &dstrNoDeleteFileName ) );
        pwszFileName = dstrNoDeleteFileName.pwszString;
    }

	// Parse the LID from argv[0]
    dsstrTemp.m_cch = DRMCRT_strlen( argv[0] );
    dsstrTemp.m_ich = 0;
    ChkDR( DRM_B64_DecodeA( argv[0], &dsstrTemp, &cbLID, idLID.rgb, 0 ) );

    ChkDR( TST_CheckForLicenseSecureStoreEntry( (DRM_WCHAR *)pwszFileName, &idLID ) );

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    return dr;
}

/**************************************************************************************************************
** Function:    TestVerifyDecryptorSetupFlag
**
** Synopsis:    Loads PRO data from file and verifies decryptor setup value.
**              
**
** Arguments:   argv[0] - file containing PRO data
**              argv[1] - expected decryptor setup value: 1 = ONDEMAND or NULL (if not found)
**
**************************************************************************************************************/
DRM_RESULT DRM_CALL TestVerifyDecryptorSetupFlag( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_BYTE        *pbPRO            = NULL;
    DRM_DWORD        cbPRO            = 0;
    DRM_BYTE        *pbWRMHeader      = NULL;
    DRM_DWORD        cbWRMHeader      = 0;
    DRM_STRING       dstrWRMHeaderStr = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDecryptorStr = EMPTY_DRM_STRING;

    ChkArg( argc == 2 );

    ChkBOOL( LoadTestFile( NULL, argv[0], (DRM_BYTE **)&pbPRO, &cbPRO ), DRM_E_TEST_FILE_LOAD_ERROR );
    
    ChkDR( DRM_PRO_GetRecord( pbPRO, 
                              cbPRO,
                              PLAYREADY_WRMHEADER,
                             &pbWRMHeader,
                             &cbWRMHeader ) );

    dstrWRMHeaderStr.pwszString = (DRM_WCHAR*)pbWRMHeader;
    dstrWRMHeaderStr.cchString = cbWRMHeader / SIZEOF(DRM_WCHAR);

    ChkDR( DRM_HDR_GetAttribute( (DRM_CONST_STRING*)&dstrWRMHeaderStr,
                                  NULL,
                                  DRM_HEADER_ATTRIB_DECRYPTOR_SETUP,
                                 &dstrDecryptorStr,
                                  g_wchForwardSlash ) );

    if ( argv[1] != NULL )
    {
        if ( atoi( argv[1] ) == 1 )
        {
            ChkBOOL( DRM_UTL_DSTRStringsEqual( &dstrDecryptorStr, &g_dstrTagONDEMAND ), DRM_E_TEST_DATA_VERIFICATION_FAILURE );
        }
        else
        {
            ChkArg(FALSE);
        }
    }
    else
    {
        ChkBOOL( dstrDecryptorStr.cchString == 0, DRM_E_TEST_DATA_VERIFICATION_FAILURE );
    }

ErrorExit:
    return dr;
}


/**************************************************************************************
** Function:    TestAddLicenseToPRO
**
** Synopsis:    Adds one license to embedded license store in the PRO.
**
** Arguments:   argv[0] - KID or "HEADER_KID" to use current one
**              argv[1] - LID
**              argv[2] - name of test variable holding a license to be embedded
**
** Returns:     DRM_SUCCESS - Successfully embedded a license.
**
** Notes:       1. PRO should already be set up via SetContentHeader API.
**              2. Invoke TestManagerCreateXMRLicenseFromPool()
**                 to put a license binary blob into test variable.
**************************************************************************************/
DRM_RESULT DRM_CALL TestAddLicenseToPRO( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                 dr                                               = DRM_SUCCESS;
    DRM_APP_CONTEXT           *poManagerContext                                 = NULL;
    DRM_APP_CONTEXT_INTERNAL  *poAppContextInternal                             = NULL;
    DRM_WCHAR                  rgwcKID[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = { 0 };
    DRM_WCHAR                  rgwcLID[ 256 ]                                   = { 0 };
    DRM_STRING                 dstrKID                                          = EMPTY_DRM_STRING;
    DRM_STRING                 dstrLID                                          = EMPTY_DRM_STRING;
    DRM_BYTE                  *pbLicenseVar                                     = NULL;
    DRM_DWORD                 *pcbLicense                                       = NULL;
    
    GET_SHARED_APP_CONTEXT( poManagerContext );
    ChkArg( poManagerContext != NULL );
   
    poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL && argv[1] != NULL && argv[2] != NULL );
    
    /*
    ** Verify that a PlayReady Object with an embedded store record
    ** has been set using Drm_Content_SetProperty.
    */
    ChkBOOL( poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET
             && poAppContextInternal->cbTotalHeaderData > 0, DRM_E_TEST_PRO_HEADER_NOT_SET);
    ChkBOOL( ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
                 || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
             && poAppContextInternal->cbEmbeddedStore > 0
             && poAppContextInternal->pbEmbeddedStore != NULL, DRM_E_TEST_NON_PRO_HEADER_TYPE );

    /*
    ** Retrieve the KID from the Header if told to do so,
    ** otherwise read explicit value
    */
    if ( DRMCRT_strncmp( argv[0], "HEADER_KID", 10 ) == 0 )
    {
        DSTR_FROM_PB( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                       poAppContextInternal->pwszHeaderData,
                       poAppContextInternal->cbHeaderData );
        ChkDR( DRM_HDR_GetAttribute( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                      NULL,
                                      DRM_HEADER_ATTRIB_KID,
                                      (DRM_CONST_STRING*)&dstrKID,
                                      0 ) );
    }
    else
    {
        dstrKID.pwszString = rgwcKID;
        dstrKID.cchString = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

        ChkArg( DRMCRT_strlen( argv[0] ) <= NO_OF( rgwcKID ) );
        ChkDR( DRM_STR_UTF8toDSTR( ( DRM_CHAR* )( argv[0] ), DRMCRT_strlen( argv[0] ), &dstrKID ) );
    }

    /*
    ** LID
    */
    TST_UTL_mbstowcs( rgwcLID, argv[1], strlen( argv[1] ) + 1 );
    dstrLID.pwszString = rgwcLID;
    dstrLID.cchString = ( DRM_DWORD )strlen( argv[1] );

    /* 
    ** License data buffer 
    */
    MAP_TO_GLOBAL_TEST_VAR_EX( pbLicenseVar, pcbLicense, argv[2] );
    ChkBOOL( pbLicenseVar != NULL && *pcbLicense > 0, DRM_E_TEST_INVALIDARG );

    ChkDR( TST_UTL_TestAddLicenseBufferToPRO( (DRM_CONST_STRING*)&dstrKID,
                                              (DRM_CONST_STRING*)&dstrLID,
                                               (const DRM_BYTE *)pbLicenseVar,
                                              *pcbLicense,
                                               (poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID ?
                                                   poAppContextInternal->pbDRMHeaderData + CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR )
                                                 : poAppContextInternal->pbDRMHeaderData),
                                               (poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID ?
                                                   poAppContextInternal->cbPROData - CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR )
                                                 : poAppContextInternal->cbPROData ) ) );

ErrorExit:
    return dr;
}


/*************************************************************************************************
** Function:    _TstCreateWRMHeaderFromPoolHelper
**
** Synopsis:    Helper function to read data file and invoke headergen library.
**
** Arguments: f_pszHeaderPoolName - header test data file name.
**            f_pszHeaderId       - header test data ID.
**            f_pOverrideParams   - structure containing optional overrides of test data.
**            f_ppbHeader         - pointer to output byte buffer containing WRMHeader.
**                                  Needs to be freed by API caller.
**            f_pcbHeader         - size of *f_ppbHeader
**
** Returns:   DRM_SUCCESS  - on success
*************************************************************************************************/
DRM_RESULT _TstCreateWRMHeaderFromPoolHelper( __in  const DRM_CHAR               *f_pszHeaderPoolName,
                                              __in  const DRM_CHAR               *f_pszHeaderId,
                                              __in_opt    TestWRMHeaderOverrides *f_pOverrideParams,
                                              __out       DRM_BYTE              **f_ppbHeader,
                                              __out       DRM_DWORD              *f_pcbHeader )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_BYTE        *pbPool     = NULL;
    DRM_BYTE        *pbPoolNull = NULL;
    DRM_DWORD        cbPool     = 0;
    DRM_CONST_STRING dstrPool   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPoolID = EMPTY_DRM_STRING;

    ChkArg( f_pszHeaderPoolName != NULL );
    ChkArg( f_pszHeaderId != NULL );
    ChkArg( f_ppbHeader != NULL );
        
    ChkDR( MakeDRMString(&dstrPoolID, f_pszHeaderId) );
    ChkMem( dstrPoolID.pwszString );

    /*
    ** Load data from the file
    */    
    ChkBOOL( LoadTestFile( "files", f_pszHeaderPoolName, &pbPool, &cbPool ) || LoadTestFile( NULL, f_pszHeaderPoolName, &pbPool, &cbPool ), DRM_E_FILENOTFOUND );

    /* 
    ** NULL-terminate pbPool before calling ChkDR( MakeDRMString() )
    */
    ChkMem( pbPoolNull = ( DRM_BYTE * )Oem_MemAlloc( cbPool + SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( pbPoolNull, cbPool + SIZEOF( DRM_WCHAR ) );
    DRM_BYT_CopyBytes( pbPoolNull, 0, pbPool, 0, cbPool );

    ChkDR( MakeDRMString( &dstrPool, ( DRM_CHAR * )pbPoolNull ) );
    ChkMem( dstrPool.pwszString );

    dr = TST_UTL_TestGenerateWRMHeader( &dstrPool, 
                                        &dstrPoolID, 
                                         NULL,
                                         f_pcbHeader,
                                         f_pOverrideParams );

    ChkBOOL( DRM_E_BUFFERTOOSMALL == dr, dr );

    ChkMem( *f_ppbHeader = ( DRM_BYTE * )Oem_MemAlloc( *f_pcbHeader ) );

    ChkDR( TST_UTL_TestGenerateWRMHeader( &dstrPool, 
                                          &dstrPoolID, 
                                          *f_ppbHeader,
                                           f_pcbHeader,
                                           f_pOverrideParams ) );

ErrorExit:
    FREEDRMSTRING( dstrPool );
    FREEDRMSTRING( dstrPoolID );
    SAFE_OEM_FREE( pbPoolNull );
    SAFE_OEM_FREE( pbPool );
    
    return dr;
}


/******************************************************************************************************************
** Function: TestCreateWRMHeaderFromPool
**
** Synopsis:
**
** Arguments: argv[0]  - Test variable name to hold the header data
**            argv[1]  - Header data pool file name
**            argv[2]  - ID within the file
**            argv[3]  - KID (optional for v.4.1)
**            argv[4]  - Content key used to generate checksum (optional)
**            argv[5]  - Cipher type (optional). Valid values: AESCTR, COCKTAIL, INVALID
**            argv[6]  - LA URL (optional)
**            argv[7]  - LUI URL (optional)
**            argv[8]  - Domain service ID (optional)
**            argv[9]  - Custom attributes (optional)
**            argv[10] - Decryptor Setup (optional). If supplied will cause V4.1 header to be generated, else v4.0
**                       Valid values: ONDEMAND or NULL
**            argv[11] - Version override: "v4" or "v4.1" (optional)
**
** Returns: DRM_SUCCESS - on success
********************************************************************************************************************/
DRM_RESULT DRM_CALL TestCreateWRMHeaderFromPool( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT              dr           = DRM_SUCCESS;
    DRM_WCHAR               rgwcKID[256] = {0};
    DRM_WCHAR               rgwcLAURL[256] = {0};
    DRM_WCHAR               rgwcLUIURL[256] = {0};
    DRM_WCHAR               rgwcDS_ID[256] = {0};
    DRM_WCHAR               rgwcCustom[256] = {0};
    DRM_WCHAR               rgwcDecryptor[256] = {0};
    DRM_WCHAR               rgwcCipherType[256] = {0};
    DRM_WCHAR               rgwcContentKey[256] = {0};
    DRM_WCHAR               rgwcVersion[256] = {0};
    DRM_BYTE               *pbVar        = NULL;
    DRM_BYTE               *pbHeaderData = NULL;
    DRM_DWORD               cbHeaderData = 0;
    TestWRMHeaderOverrides *pParams      = NULL;

    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkMem( pParams = ( TestWRMHeaderOverrides * )Oem_MemAlloc( SIZEOF( TestWRMHeaderOverrides ) ) );
    ZEROMEM( pParams, SIZEOF( TestWRMHeaderOverrides ) );

    /*
    ** read TestWRMHeaderOverrides params
    */

    if ( argc > 3 && argv[3] != NULL )
    {
        TST_UTL_mbstowcs( rgwcKID, argv[3], strlen( argv[3] ) + 1 );
        pParams->dstrKID.pwszString = rgwcKID;
        pParams->dstrKID.cchString = ( DRM_DWORD )strlen( argv[3] );
        pParams->bOverrideSet = TRUE;
    }
    if ( argc > 4 && argv[4] != NULL )
    {
        TST_UTL_mbstowcs( rgwcContentKey, argv[4], strlen( argv[4] ) + 1 );
        pParams->dstrContentKey.pwszString = rgwcContentKey;
        pParams->dstrContentKey.cchString = ( DRM_DWORD )strlen( argv[4] );
        pParams->bOverrideSet = TRUE;
    }
    if ( argc > 5 && argv[5] != NULL )
    {
        TST_UTL_mbstowcs( rgwcCipherType, argv[5], strlen( argv[5] ) + 1 );
        pParams->dstrCipherType.pwszString = rgwcCipherType;
        pParams->dstrCipherType.cchString = ( DRM_DWORD )strlen( argv[5] );
        pParams->bOverrideSet = TRUE;
    }
    if ( argc > 6 && argv[6] != NULL )
    {
        TST_UTL_mbstowcs( rgwcLAURL, argv[6], strlen( argv[6] ) + 1 );
        pParams->dstrPlayReadySilentURL.pwszString = rgwcLAURL;
        pParams->dstrPlayReadySilentURL.cchString = ( DRM_DWORD )strlen( argv[6] );
        pParams->bOverrideSet = TRUE;
    }
    if ( argc > 7 && argv[7] != NULL )
    {
        TST_UTL_mbstowcs( rgwcLUIURL, argv[7], strlen( argv[7] ) + 1 );
        pParams->dstrPlayReadyNonSilentURL.pwszString = rgwcLUIURL;
        pParams->dstrPlayReadyNonSilentURL.cchString = ( DRM_DWORD )strlen( argv[7] );
        pParams->bOverrideSet = TRUE;
    }
    if ( argc > 8 && argv[8] != NULL )
    {
        TST_UTL_mbstowcs( rgwcDS_ID, argv[8], strlen( argv[8] ) + 1 );
        pParams->dstrServiceID.pwszString = rgwcDS_ID;
        pParams->dstrServiceID.cchString = ( DRM_DWORD )strlen( argv[8] );
        pParams->bOverrideSet = TRUE;
    }    
    if ( argc > 9 && argv[9] != NULL )
    {
        TST_UTL_mbstowcs( rgwcCustom, argv[9], strlen( argv[9] ) + 1 );
        pParams->dstrCustomAttriburesXML.pwszString = rgwcCustom;
        pParams->dstrCustomAttriburesXML.cchString = ( DRM_DWORD )strlen( argv[9] );
        pParams->bOverrideSet = TRUE;
    }    
    if ( argc > 10 && argv[10] != NULL )
    {
        TST_UTL_mbstowcs( rgwcDecryptor, argv[10], strlen( argv[10] ) + 1 );
        pParams->dstrDecryptorSetup.pwszString = rgwcDecryptor;
        pParams->dstrDecryptorSetup.cchString = ( DRM_DWORD )strlen( argv[10] );
        pParams->bOverrideSet = TRUE;
    }    
    if ( argc > 11 && argv[11] != NULL )
    {
        TST_UTL_mbstowcs( rgwcVersion, argv[11], strlen( argv[11] ) + 1 );
        pParams->dstrWRMHeaderVersion.pwszString = rgwcVersion;
        pParams->dstrWRMHeaderVersion.cchString = ( DRM_DWORD )strlen( argv[11] );
        pParams->bOverrideSet = TRUE;
    }    

    ChkDR( _TstCreateWRMHeaderFromPoolHelper( argv[1], argv[2], pParams, &pbHeaderData, &cbHeaderData ) );
    
    /*
    ** copy result into test variable
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbVar, argv[0], cbHeaderData );
    MEMCPY( pbVar, pbHeaderData, cbHeaderData );

ErrorExit:
    SAFE_OEM_FREE( pParams );
    SAFE_OEM_FREE( pbHeaderData );
    return dr;
}


/**********************************************************************
**
** Function: TestCreatePlayReadyDRMHeader   
**
** Synopsis: Tests the creation of PlayReady header.
**              
**
** Arguments: 
**            argc     - number of arguments
**            argv[0]  - Path to the V4 XML file - or name of test variable holding WRMHeader data    
**            argv[1]  - Flag what to pass for size of PlayReady header. NULL - means detect from BUFFER_TOO_SMALL. 
**                          Otherwize use provided value in argv[2]
**            argv[2]  - Expected cipher type. If it is not NULL, the value is compared against the cipher type in the
**                          generated PlayReady header.
**            argv[3]  - file name to save resultant PRO (optional)
**
** Returns:     DRM_SUCCESS on success. 
**              DRM_E_INVALIDARG for invalid arguments
**              DRM_E_FAIL if PlayReady header fails validation.
**              Other errors returned by DRM_PRO_Create
**              
**            
**********************************************************************/
DRM_RESULT DRM_CALL TestCreatePlayReadyDRMHeader( __in long argc, __in_ecount( argc ) char **argv )

{   DRM_RESULT            dr                  = DRM_SUCCESS;
    DRM_BYTE             *pbWRMHeader         = NULL;
    DRM_DWORD             dwWRMHeader         = 0;
    DRM_DWORD             dwTestParams        = TEST_PRO_NoSpecialTestcases;
    DRM_BYTE             *pbPlayReadyHeader   = NULL;
    DRM_DWORD             cbPlayReadyHeader   = 0;
    DRM_BYTE             *pbWRMHeaderVar      = NULL;
    DRM_DWORD            *pcbWRMHeaderVar     = NULL;
    DRM_WCHAR            *pwszV4Header        = 0;
    DRM_DWORD             dwV4HeaderSize      = 0;
    DRM_BYTE             *pbEmbeddedStore     = NULL;
    DRM_DWORD             dwEmbedStoreSize    = 0;
    DRM_SUPPORTED_CIPHERS eExpectedCipherType = eDRM_CIPHER_INVALID;
    DRM_SUPPORTED_CIPHERS eActualCipherType   = eDRM_CIPHER_INVALID;
    
    /*
    ** All parameters except the last one should be provided.
    */
    ChkArg( argc >= 3 );

    /*
    ** If argv[0] is NULL, then no file meaning to build a special case of PRO without WRMHeader
    */
    if ( argv[0] != NULL )   
    {
        if ( !LoadTestFile( NULL, argv[0], (DRM_BYTE **)&pbWRMHeader, &dwWRMHeader ) )
        {
            /*
            ** see if test variable is present and holds any data and copy it into pbWRMHeader.
            ** Need copying because pbWRMHeader will be freed at the end of this function.
            */
            MAP_TO_GLOBAL_TEST_VAR_EX( pbWRMHeaderVar, pcbWRMHeaderVar, argv[0] );

            ChkMem(pbWRMHeader = (DRM_BYTE*) Oem_MemAlloc(*pcbWRMHeaderVar) );
            MEMCPY( (DRM_BYTE*)pbWRMHeader, pbWRMHeaderVar, *pcbWRMHeaderVar);
            dwWRMHeader = *pcbWRMHeaderVar;
        }
    }
    else
    {
        dwTestParams = TEST_PRO_NoWRMHeaderRecord;
    }
    
    /*
    ** This would be the size of the header. If it is NULL, we get the size from the call of DRM_PRO_Create().
    */
    if ( argv[1] != NULL )
    {
        cbPlayReadyHeader = atoi( argv[1] );
    }
    else
    {   
        cbPlayReadyHeader = 0;
    }

    if ( argv[2] != NULL )
    {
        eExpectedCipherType = ( DRM_SUPPORTED_CIPHERS )atoi( argv[2] );
    }    
    
    ChkDR( TST_UTL_TestGeneratePRO( pbWRMHeader,
                                    dwWRMHeader,
                                    dwTestParams,
                                   &pbPlayReadyHeader,
                                   &cbPlayReadyHeader ) );

    /*
    **  If API succeded, we try to verify that V4 XML object is inserted into PlayReady object - unless it was some special case
    */

    if ( dwTestParams == 0 )
    {
        /*
        ** Validates PlayReady object and extracts V4 XML from there. 
        */
        ChkDR( DRM_PRO_GetRecord( pbPlayReadyHeader, cbPlayReadyHeader, PLAYREADY_WRMHEADER, (DRM_BYTE **)&pwszV4Header, &dwV4HeaderSize ) );
        /*
        ** Validates that V4 XML is the same as was inserted.
        */
        ChkBOOL( dwV4HeaderSize == dwWRMHeader, DRM_E_FAIL );
        ChkBOOL( 0 == MEMCMP( pwszV4Header, pbWRMHeader, dwWRMHeader ), DRM_E_FAIL );

        /*
        ** Check that embedded store is present:
        */
        ChkDR( DRM_PRO_GetRecord( pbPlayReadyHeader, cbPlayReadyHeader, PLAYREADY_EMBEDDED_LICENSE_STORE, &pbEmbeddedStore, &dwEmbedStoreSize ) );
        ChkBOOL( dwEmbedStoreSize == MAX_EMBEDDED_STORE_LEN, DRM_E_FAIL  );

        if ( eExpectedCipherType != eDRM_CIPHER_INVALID )
        { 
            ChkDR( DRM_PRO_GetCipherType( pbPlayReadyHeader, cbPlayReadyHeader, &eActualCipherType ) );
            ChkBOOL( eActualCipherType == eExpectedCipherType, DRM_E_FAIL );
        }
    }
    if ( argc == 4 && argv[3] != NULL )
    {
        /*
        ** Save PRO into a file
        */
        DRM_STRING dstrOutFilename = EMPTY_DRM_STRING;
        ChkDR( TST_UTL_MakePathString( NULL, argv[3], (DRM_CONST_STRING*)&dstrOutFilename ) );
        ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, pbPlayReadyHeader, cbPlayReadyHeader ), DRM_E_FILEWRITEERROR );
    }

ErrorExit:
    SAFE_OEM_FREE ( pbPlayReadyHeader );
    SAFE_OEM_FREE ( pbWRMHeader );

    return dr;
}


/**************************************************************************************************
** Function:    TestSetupDecryptor
**
** Synopsis:    Reinitialize DECRYPT_CONTEXT_VAR test variable with specified decryptor data
**
** Arguments:   argv[0] - name of test variable holding decryptor data
**
**
**************************************************************************************************/
DRM_RESULT DRM_CALL TestSetupDecryptor( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_DECRYPT_CONTEXT *pDecryptor = NULL;
    DRM_DECRYPT_CONTEXT *pGlobal    = NULL;
    

    ChkArg( argc == 1 );    
    ChkArg( argv[0] != NULL );

    MAP_TO_GLOBAL_TEST_VAR( pDecryptor, argv[0] );
    MAP_TO_GLOBAL_TEST_VAR( pGlobal, DECRYPT_CONTEXT_VAR );

    MEMCPY(pGlobal, pDecryptor, SIZEOF(DRM_DECRYPT_CONTEXT) );
    
ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL MODULE_PRETESTCASE( LiveTV )(
    __in long lTCID, 
    __in const char *strTCName )
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( LiveTV )(
    __in long lTCID, 
    __in const char *strTCName )
{
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(LiveTV)
    API_ENTRY( TestStartLicense )
    API_ENTRY( TestFinishLicense )
    API_ENTRY( TestUnpackLicense )
    API_ENTRY( TestVerifyLicense )
    API_ENTRY( TestAddAuxKeyObject )
    API_ENTRY( TestAddUplinkXObject )
    API_ENTRY( TestVerifyStoreSlotCount )
	API_ENTRY( TestVerifySecureStoreEntry )
    API_ENTRY( TestAddLicenseToPRO )
    API_ENTRY( TestVerifyDecryptorSetupFlag )
    API_ENTRY( TestCreatePlayReadyDRMHeader )
    API_ENTRY( TestSetupDecryptor )
    API_ENTRY( TestCreateWRMHeaderFromPool )
	API_ENTRY( Test_ChangeTimeSeconds)
	API_ENTRY( TestManagerPrepareOPLCallback )
END_TEST_API_LIST
    EXIT_PKTEST_NAMESPACE_CODE

