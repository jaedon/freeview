/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <drmkeygenerator.h>
#include <oemrsa.h>
#include <drmxmlbuilder.h>

#include <drmsoapxmlconstants.h>
#include <drmlicacqv3constants.h>
#include <drmxmlparser.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>

#include <drmhdrbuilder.h>

#include <drmxmr.h>
#include <drmxmrbuilder.h>
#include <drmbcert.h>
#include <drmbcertparser.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    TestManagerInitResponse
**
** Synopsis:
**
** Arguments:   argv[0] : Optional. Protocol Type, V2 or V3, default to V2
**              argv[1] : Optional. Symmetric Optimization Options:
**                        NULL      -- Don't Optimize
**                        OptNewSLK -- Optimize the licenses with a New SLK
**                        OptOldSLK -- Optimize the licenses with the SLK
**                                     from the previous call to the API
**
** Returns:     DRM_SUCCESS         - on success
**
** Note:        If OptOldSLK is passed in as the second argument, but no SLK
**              can be found stored in the global test variables, a new SLK
**              is created.
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr         = DRM_SUCCESS;

    DRM_BYTE  *pbSlk      = NULL;
    DRM_DWORD  cbSlk      = 0;
    DRM_BYTE  *pbSlkData  = NULL;
    DRM_DWORD  cbSlkData  = 0;

    DRM_BOOL   fUseSlk   = FALSE;
    DRM_BOOL   fSymmOpt  = FALSE;
    DRM_BOOL  *pfUseSlk  = NULL;
    DRM_BOOL  *pfSymmOpt = NULL;
    DRM_BOOL   fNewSlk   = FALSE;


     if( argc > 1 && argv[1] != NULL  )
     {
         fSymmOpt = TRUE;
         if( 0 == strncmp( argv[1], "OptNewSLK", 9 ) )
         {
             fNewSlk = TRUE;
             fUseSlk = TRUE;
         }
         else if( 0 == strncmp( argv[1], "OptOldSLK", 9 ) )
         {
             fNewSlk = FALSE;
             fUseSlk = TRUE;
         }
     }

    if( argc == 0 || strncmp( argv[0], "V2", 2 ) == 0 )
    {
        /*
        ** initialize the response xml frame
        */
        _XMBContext *pbResponseContext  = NULL;

        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbResponseContext, RESPONSE_CONTEXT_VAR, RESPONSECONTEXTSIZE );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pfSymmOpt, RESPONSE_SYMM_OPT, SIZEOF( DRM_BOOL ) );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pfUseSlk,  RESPONSE_USE_SLK,  SIZEOF( DRM_BOOL ) );

        *pfSymmOpt = fSymmOpt;
        *pfUseSlk  = fUseSlk;

        ChkDR( TestResponseInit( pbResponseContext, RESPONSECONTEXTSIZE ) );

        if( *pfSymmOpt )
        {

            DRM_BYTE  *pbSlkKey   = NULL;
            DRM_DWORD  cbSlkKey   = DES_KEYSIZE;
            DRM_DWORD *pcbSlkKey  = &cbSlkKey;
            DRM_CHAR  *pbSlkDataEnc  = NULL;
            DRM_DWORD  cbSlkDataEnc  = ( SLK_DATA_SIZE_IN_CHARS + 1 ) * SIZEOF(DRM_CHAR);
            DRM_DWORD *pcbSlkDataEnc = &cbSlkDataEnc;
            DRM_CHAR  *pszSlkId      = NULL;
            DRM_DWORD  cbSlkId       = ( SLK_ID_SIZE_IN_CHARS + 1 ) * SIZEOF(DRM_CHAR);
            DRM_DWORD *pcbSlkId      = &cbSlkId;

            dr = DRM_TST_GetTestVar( RESPONSE_V2_SLK, (DRM_VOID**) &pbSlkKey, &pcbSlkKey );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSlkKey, RESPONSE_V2_SLK, cbSlkKey );
            }

            dr = DRM_TST_GetTestVar( RESPONSE_V2_SLK_DATA, (DRM_VOID**) &pbSlkDataEnc, &pcbSlkDataEnc );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSlkDataEnc, RESPONSE_V2_SLK_DATA, cbSlkDataEnc );
            }

            dr = DRM_TST_GetTestVar( RESPONSE_V2_SLK_ID, (DRM_VOID**) &pszSlkId, &pcbSlkId );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                MAP_TO_GLOBAL_TEST_VAR_Alloc( pszSlkId, RESPONSE_V2_SLK_ID, cbSlkId );
            }

            if( fNewSlk )
            {
                DRM_ID                    oSlkID         = {0};
                DRM_DWORD                 cchSlkDataEnc  = *pcbSlkDataEnc;
                DRM_APP_CONTEXT_INTERNAL *poAppContext   = NULL;
                GET_SHARED_APP_CONTEXT( poAppContext );
                pbSlk      = NULL;
                cbSlk      = DRM_ECC160_PLAINTEXT_LEN;
                pbSlkData = NULL;
                cbSlkData = DRM_ECC160_CIPHERTEXT_LEN;


                ChkMem( pbSlk = (DRM_BYTE*)Oem_MemAlloc( cbSlk ) );
                ZEROMEM( pbSlk, cbSlk );

                ChkMem( pbSlkData = (DRM_BYTE*)Oem_MemAlloc( cbSlkData ) );
                ZEROMEM( pbSlkData, cbSlkData );

                ChkDR( Oem_Random_GetBytes( NULL, pbSlkKey, cbSlkKey ) );
                MEMCPY( pbSlk, pbSlkKey, cbSlkKey );
                ChkDR( DRM_PK_Encrypt( NULL,
                                       ( DRM_BYTE * )&(poAppContext->oBlackBoxContext.CryptoContext),
                                       eDRM_ECC_P160,
                                       DRM_ECC160_PUBLIC_KEY_LEN,
                                       ( DRM_BYTE * )&(poAppContext->oBlackBoxContext.cachedCertValues.pubkeyCurrent),
                                       cbSlk,
                                       pbSlk,
                                       &cbSlkData,
                                       pbSlkData ) );

                ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)pbSlkData, cbSlkData, pbSlkDataEnc, &cchSlkDataEnc, 0 ) );
                ChkArg( *pcbSlkDataEnc > cchSlkDataEnc );
                PUT_CHAR( pbSlkDataEnc, cchSlkDataEnc, '\0' );

                /*
                ** Create and Store a SLK ID
                */
                ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE *) &oSlkID, SIZEOF( DRM_ID ) ) );
                ChkDR( DRM_B64_EncodeA((DRM_BYTE *) &oSlkID, SIZEOF( DRM_ID ), pszSlkId, pcbSlkId, 0));
                ChkDR(DRM_SUCCESS);

            }
        }


    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pfSymmOpt, RESPONSE_SYMM_OPT, SIZEOF( DRM_BOOL ) );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pfUseSlk,  RESPONSE_USE_SLK,  SIZEOF( DRM_BOOL ) );

        *pfSymmOpt = fSymmOpt;
        *pfUseSlk  = fUseSlk;

        if( *pfSymmOpt )
        {
            DRM_CHAR  *pszSlkId   = NULL;
            DRM_DWORD  cbSlkId    = ( SLK_ID_SIZE_IN_CHARS + 1 ) * SIZEOF(DRM_CHAR);
            DRM_DWORD *pcbSlkId   = &cbSlkId;
            DRM_CHAR  *pszSlkData = NULL;
            DRM_DWORD  cbSlkData  = ( SLK_DATA_SIZE_IN_CHARS + 1 ) * SIZEOF(DRM_CHAR);
            DRM_DWORD *pcbSlkData = &cbSlkData;
            DRM_APP_CONTEXT_INTERNAL *poAppContext = NULL;
            DRM_DWORD *pcbSlk     = &cbSlk;
            pbSlk      = NULL;
            cbSlk      = DRM_AES_KEYSIZE_128;

            GET_SHARED_APP_CONTEXT( poAppContext );

            dr = DRM_TST_GetTestVar( RESPONSE_V3_SLK, (DRM_VOID**) &pbSlk, &pcbSlk );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                ChkDR( DRM_TST_AllocTestVar( RESPONSE_V3_SLK, cbSlk, (DRM_VOID**) &pbSlk, &pcbSlk ) );
            }
            else
            {
                ChkDR( dr );
            }

            dr = DRM_TST_GetTestVar( RESPONSE_V3_SLK_ID, (DRM_VOID**) &pszSlkId, &pcbSlkId );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                ChkDR( DRM_TST_AllocTestVar( RESPONSE_V3_SLK_ID, cbSlkId, (DRM_VOID**) &pszSlkId, &pcbSlkId ) );
            }
            else
            {
                ChkDR( dr );
            }

            dr = DRM_TST_GetTestVar( RESPONSE_V3_SLK_ENCRYPTED, (DRM_VOID**) &pszSlkData, &pcbSlkData );
            if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
            {
                fNewSlk = TRUE;
                ChkDR( DRM_TST_AllocTestVar( RESPONSE_V3_SLK_ENCRYPTED, cbSlkData, (DRM_VOID**) &pszSlkData, &pcbSlkData ) );
            }
            else
            {
                ChkDR( dr );
            }

            if( fNewSlk )
            {
                PLAINTEXT_P256  eccKey     = { 0 };
                DRM_ID          oSlkID     = { 0 };
                CIPHERTEXT_P256 encSymmKey = { 0 };
                PUBKEY_P256     machineKey = { 0 };
                DRM_DWORD       cchSlkData = *pcbSlkData;
                DRM_DWORD       cchSlkId   = *pcbSlkId;

                do
                {
                    // Fill the cleartext with random bytes
                    ChkDR( Oem_Random_GetBytes(
                        NULL,
                        eccKey.m_rgbPlaintext,
                        ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) );

                    dr = OEM_ECC_GenerateHMACKey_P256(
                        &eccKey,
                        (struct bigctx_t*) &poAppContext->oBlackBoxContext.CryptoContext );

                } while( dr == DRM_E_P256_HMAC_KEYGEN_FAILURE );

                /*
                ** The first half of the session key is just padding
                ** The second half is the AES key used to encrypt the content key
                */
                MEMCPY( pbSlk, &(eccKey.m_rgbPlaintext[DRM_AES_KEYSIZE_128]), DRM_AES_KEYSIZE_128 );

                /*
                ** Store the Symmetric Key as encrypted for the device.
                */

                MEMCPY( &machineKey,
                        &(poAppContext->oBlackBoxContext.cachedBCertValues.DeviceKeyEncrypt.PublicKey),
                        SIZEOF( PUBKEY_P256 ) );

                ChkDR( OEM_ECC_Encrypt_P256(
                   &machineKey,
                   &eccKey,
                   &encSymmKey,
                   (struct bigctx_t*) &poAppContext->oBlackBoxContext.CryptoContext ) );

                /*
                ** Now Base64 encode the SLK Data and store it.
                */
                ChkDR( DRM_B64_EncodeA( (DRM_BYTE *) &encSymmKey,
                                         SIZEOF( CIPHERTEXT_P256 ),
                                         pszSlkData,
                                        &cchSlkData,
                                         0) );

                ChkBOOL( *pcbSlkData > cchSlkData, DRM_E_TEST_UNEXPECTED_OUTPUT );

                PUT_CHAR( pszSlkData, cchSlkData, '\0' );

                /*
                ** Create and Store a SLK ID
                */
                ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE *) &oSlkID, SIZEOF( DRM_ID ) ) );

                ChkDR( DRM_B64_EncodeA(  (DRM_BYTE *) &oSlkID,
                                         SIZEOF( DRM_ID ),
                                         pszSlkId,
                                        &cchSlkId,
                                         0) );

                ChkBOOL( *pcbSlkId > cchSlkId, DRM_E_TEST_UNEXPECTED_OUTPUT );

                PUT_CHAR( pszSlkId, cchSlkId, '\0' );
            }
        }
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetExtensiblePolicy
**
** Synopsis:
**
** Arguments:   argv[0] : Boolean where TRUE is for right, and FALSE is for restriction
**              argv[1] : XPol object type, read as a base-10 word
**              argv[2] : Boolean indicating duplicate is allowed
**              argv[3] : Parent object type
**              argv[4] : XMR flags, read as a base-10 dword
**              argv[5] : Optional.  Secure state type (2-count/long, 3-date, or 4-binary). Ignored for right or if NULL
**              argv[6] : Optional.  Secure state data, read as DWORD for count and date, and b64-encoded for binary.  Ignored if sercure state type is being ignored.
**              argv[7] : Optional.  Policy data.  Ignored for right.
**
** Returns:     DRM_SUCCESS         - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetExtensiblePolicy( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr                      = DRM_SUCCESS;
    DRM_TEST_XPOL_DATA  *poXPolData             = NULL;
    DRM_DWORD           dwTemp                  = 0;
    DRM_BOOL            fRight                  = 0;
    DRM_UINT64          ui64SecureStateDate     = DRM_UI64(0);
    DRM_CONST_STRING    dstrXPolData            = EMPTY_DRM_STRING;
    DRMFILETIME         ftCurrentTime           = {0};

    ChkArg( argc > 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL );

    /*
    ** Check if this is a right or a restriction
    */
    ChkDR( StringToBool( argv[0], &fRight ) );

    if ( fRight )
    {
        MAP_TO_GLOBAL_TEST_VAR( poXPolData, RESPONSE_XPOL_RIGHT_VAR );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( poXPolData, RESPONSE_XPOL_RESTRICTION_VAR );
    }

    /*
    ** Free any previously allocated memory
    */
    SAFE_OEM_FREE( poXPolData->pStateDateCountorBinary );
    poXPolData->pStateDateCountorBinary = NULL;
    SAFE_OEM_FREE( poXPolData->pbXPolData );
    poXPolData->pbXPolData = NULL;

    /*
    ** Set restriction type
    */
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwTemp ) );
    poXPolData->wObjectType = dwTemp;

    /*
    ** Set duplicate allowed flag
    */
    ChkDR( StringToBool( argv[2], &(poXPolData->fDuplicateAllowed) ) );

    /*
    ** Set parent type
    */
    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &dwTemp ) );
    poXPolData->wParent = dwTemp;

    /*
    ** Set xmr flags
    */
    ChkDR( DRMCRT_AtoDWORD( argv[4], (DRM_DWORD)strlen( argv[4] ), 10, &dwTemp ) );
    poXPolData->wFlags = dwTemp;

    if ( argc > 6
      && argv[5] != NULL
      && argv[6] != NULL
      && !fRight )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[5], (DRM_DWORD)strlen( argv[5] ), 10, &dwTemp ) );
        poXPolData->fSecureStateType = dwTemp;
        switch ( poXPolData->fSecureStateType )
        {
        case XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT:
            ChkDR( DRMCRT_AtoDWORD( argv[6], (DRM_DWORD)strlen( argv[6] ), 10, &dwTemp ) );
            poXPolData->cStateDateCountorBinary = SIZEOF(DRM_DWORD);
            ChkMem( poXPolData->pStateDateCountorBinary = (DRM_VOID*)Oem_MemAlloc( poXPolData->cStateDateCountorBinary ) );
            MEMCPY( (DRM_BYTE*)poXPolData->pStateDateCountorBinary, (DRM_BYTE*)&dwTemp, poXPolData->cStateDateCountorBinary );
            Log( "", "TestManagerSetExtensiblePolicy Count as dword = %d", dwTemp );
            break;
        case XMR_EXTENSIBLE_RESTRICTON_STATE_DATE:
            Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
            FILETIME_TO_UI64( ftCurrentTime, ui64SecureStateDate );
            Log( "", "TestManagerSetExtensiblePolicy Date as u64 = %X", ui64SecureStateDate );

            ChkDR( DRMCRT_AtoDWORD( argv[6], (DRM_DWORD)strlen( argv[6] ), 10, &dwTemp ) );
            ui64SecureStateDate = DRM_UI64Add( ui64SecureStateDate, DRM_UI64( dwTemp ) );

            poXPolData->cStateDateCountorBinary = SIZEOF(DRM_UINT64);
            ChkMem( poXPolData->pStateDateCountorBinary = (DRM_VOID*)Oem_MemAlloc( poXPolData->cStateDateCountorBinary * SIZEOF( DRM_BYTE ) ) );
            MEMCPY( (DRM_BYTE*)poXPolData->pStateDateCountorBinary, (DRM_BYTE*)&ui64SecureStateDate, poXPolData->cStateDateCountorBinary );
            break;
        case XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY:
            ChkDR( MakeDRMString( &dstrXPolData, argv[6] ) );
            ChkBOOL( DRM_B64_DecodeW( &dstrXPolData, &dwTemp, (DRM_BYTE*)poXPolData->pStateDateCountorBinary, 0 ) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_INCOMPLETE );
            poXPolData->cStateDateCountorBinary = dwTemp;
            ChkMem( poXPolData->pStateDateCountorBinary = (DRM_VOID*)Oem_MemAlloc( poXPolData->cStateDateCountorBinary * SIZEOF( DRM_BYTE ) ) );
            dwTemp = poXPolData->cStateDateCountorBinary;
            ChkDR( DRM_B64_DecodeW( &dstrXPolData, &dwTemp, (DRM_BYTE*)poXPolData->pStateDateCountorBinary, 0 ) );
            Log( "", "TestManagerSetExtensiblePolicy bytes size = %d", dwTemp );
            LogBlob( "TestManagerSetExtensiblePolicy", "Secure Bytes:", (DRM_BYTE*)poXPolData->pStateDateCountorBinary, poXPolData->cStateDateCountorBinary );
            break;
        default:
            break;
        }
    }
    if ( argc > 7
      && argv[7] != NULL
      && !fRight )
    {
        FREEDRMSTRING( dstrXPolData );
        dstrXPolData.pwszString = NULL;
        dstrXPolData.cchString = 0;
        ChkDR( MakeDRMString( &dstrXPolData, argv[7] ) );
        ChkBOOL( DRM_B64_DecodeW( &dstrXPolData, &( poXPolData->cbXPolData ), poXPolData->pbXPolData, 0 ) == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_INCOMPLETE );
        ChkMem( poXPolData->pbXPolData = (DRM_BYTE*)Oem_MemAlloc( poXPolData->cbXPolData * SIZEOF( DRM_BYTE ) ) );
        ChkDR( DRM_B64_DecodeW( &dstrXPolData, &( poXPolData->cbXPolData ), poXPolData->pbXPolData, 0 ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrXPolData );
    return dr;
}


/*****************************************************************************
** Function:    TestManagerAddLicenseFromFileToResponse
**
** Synopsis:
**
** Arguments:   argv[0] : filename
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddLicenseFromFileToResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING pwszLicense       = EMPTY_DRM_STRING;
    DRM_DWORD        cbLicense         = 0;
    _XMBContext     *pbResponseContext = NULL;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( "files", argv[0], (DRM_BYTE**)&(pwszLicense.pwszString), &cbLicense )
          || LoadTestFile( NULL, argv[0], (DRM_BYTE**)&(pwszLicense.pwszString), &cbLicense ), DRM_E_TEST_INCOMPLETE );

    pwszLicense.cchString = cbLicense / SIZEOF(DRM_WCHAR);

    /*
    ** Get shared Response Context if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_CONTEXT_VAR, ( DRM_VOID ** ) &pbResponseContext, NULL ) ) )
    {
        pbResponseContext = NULL;
    }
    ChkDR( TestResponseAddLicense( pbResponseContext, &pwszLicense ) );

ErrorExit:
    FREEDRMSTRING( pwszLicense );
    return dr;
}

static DRM_RESULT _GenerateOffsetSystemTime( __inout_ecount(18) char* szSystemTimeString, long loffset)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRMSYSTEMTIME tSysTime   = {0};
    DRMFILETIME   mcFileTime = {0};
    DRM_UINT64    u64        = {0};
    DRM_WORD      wTemp      = 0;

    ChkArg( szSystemTimeString != NULL);
    ChkArg( loffset !=0 );

    Oem_Clock_GetSystemTimeAsFileTime(NULL, &mcFileTime);

    /*
    ** offset the file time
    */
    FILETIME_TO_UI64(mcFileTime, u64);
    if(loffset > 0)
    {
        u64 = DRM_UI64Add(u64, DRM_UI64Mul(DRM_UI64(loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    }
    else
    {
        u64 = DRM_UI64Sub(u64, DRM_UI64Mul(DRM_UI64(-loffset), DRM_UI64(FILETIME_TO_SECONDS)));
    }
    UI64_TO_FILETIME(u64, mcFileTime);

    /*
    ** convert back to system time from file time
    */
    ChkArg(Oem_Clock_FileTimeToSystemTime(&mcFileTime, &tSysTime));

    /*
    ** e.g. 20090101
    */
    wTemp = tSysTime.wYear;
    PUT_CHAR( szSystemTimeString, 3, TST_UTL_ByteValueToASCII( wTemp % 10 ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 2, TST_UTL_ByteValueToASCII( wTemp % 10 ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 1, TST_UTL_ByteValueToASCII( wTemp % 10 ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 0, TST_UTL_ByteValueToASCII( wTemp % 10 ) );

    wTemp = tSysTime.wMonth;
    PUT_CHAR( szSystemTimeString, 5, TST_UTL_ByteValueToASCII( wTemp % 10 ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 4, TST_UTL_ByteValueToASCII( wTemp % 10 ) );

    wTemp = tSysTime.wDay;
    PUT_CHAR( szSystemTimeString, 7, TST_UTL_ByteValueToASCII( wTemp % 10 ) );
    wTemp /= 10;
    PUT_CHAR( szSystemTimeString, 6, TST_UTL_ByteValueToASCII( wTemp % 10 ) );

    PUT_CHAR( szSystemTimeString, 8, '\0' );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerAddLicenseToResponse
**
** Synopsis:
**
** Arguments:   argv[0] : KID
**              argv[1] : LID
**              argv[2] : option in creating a test license
**              argv[3] : extra xml string to add to the license
**              argv[4] : content sign key
**              argv[5] : key seed
**              argv[6] : optional license type: 0(simple), 1(leaf), 2(root)
**              argv[7] : optional uplink kid
**              argv[8] : optional offset time in hours
**              argv[9] : optional NOSYMVAL - Do not add SYMVAL to the license.
**              argv[10]: optional number of SLK elements to add to the response
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddLicenseToResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING pwszLicense       = EMPTY_DRM_STRING;
    ELicenseType     eLicType          = MAKE_SIMPLE_LICENSE;
    DRM_CHAR        *pstrRandomKeyID   = NULL;
    _XMBContext     *pbResponseContext = NULL;

    DRM_BOOL        *pfUseSlk          = NULL;
    DRM_BOOL        *pfSymmOpt         = NULL;
    DRM_BOOL         fNoSymVal         = FALSE;
    DRM_DWORD        dwNumSlk          = 1;

    DRM_CHAR szExtraXML[2000]={0};

    ChkArg( argc    >  3 );
    ChkArg( argv[2] != NULL );

    if ( argc > 6 && argv[6] != NULL )
    {
        eLicType = (ELicenseType)atol( argv[6] );
    }

    /*
    ** Get shared random KID if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** ) &pstrRandomKeyID, NULL ) ) )
    {
        pstrRandomKeyID = NULL;
    }
    /*
    ** Get shared Response Context if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_CONTEXT_VAR, ( DRM_VOID ** ) &pbResponseContext, NULL ) ) )
    {
        pbResponseContext = NULL;
    }

    /*
    ** Determine if license is meant to use a slk
    */
    MAP_TO_GLOBAL_TEST_VAR( pfUseSlk,  RESPONSE_USE_SLK );

    /*
    ** Determine if license is meant to be symmetrically optimized
    */
    MAP_TO_GLOBAL_TEST_VAR( pfSymmOpt,  RESPONSE_SYMM_OPT );

    /*
    ** add encrypted and encoded license
    */
    ChkDR( TestLicGenInit( DRM_MAX_LICENSESIZE ) );

    if ( argv[0] != NULL )
    {
        if( strncmp( argv[0], "PREGENERATEDRANDOM", 18 ) == 0 && pstrRandomKeyID != NULL )
        {
            ChkDR( TestLicGenSetKeys( "LICENSE_KID", pstrRandomKeyID ) );
        }
        else
        {
            ChkDR( TestLicGenSetKeys( "LICENSE_KID", argv[0] ) );
        }
    }

    if ( argv[1] != NULL )
    {
        ChkDR( TestLicGenSetKeys( "LICENSE_LID", argv[1] ) );
    }

    if ( argc > 4 && argv[4] != NULL )
    {
        ChkDR( TestLicGenSetKeys( "CONTENT_OWNER_PUB_KEY", argv[4] ) );
    }

    if ( argc > 5 && argv[5] != NULL )
    {
        ChkDR( TestLicGenSetKeys( "LICENSE_SERVER_KEYSEED", argv[5] ) );
    }

    if ( argc > 7 && argv[7] != NULL )
    {
        ChkDR( TestLicGenSetKeys( "LICENSE_UPLINK_KID", argv[7] ) );
    }


    if ( argv[3] != NULL )
    {
        if( argc > 8 && argv[8] != NULL )
        {
            char pSystemTime[20];
            char *pszXMLEnding = "Z#]]></ACTION></ONSTORE>";
            long lOffset       = 0;
            long cchSystemTime = 0;
            long cchXMLEnding  = ( DRM_DWORD ) strlen( pszXMLEnding );
            long cchArg        = 0;

            ChkArg(argv[8]);
            lOffset=atol(argv[8])*60*60;
            ChkDR( _GenerateOffsetSystemTime( pSystemTime , lOffset) );

            cchSystemTime = ( DRM_DWORD ) strlen( pSystemTime );
            cchArg        = ( DRM_DWORD ) strlen( argv[3] );
            ChkBOOL( SIZEOF( szExtraXML ) >= ( cchSystemTime + cchArg + cchXMLEnding + 1 ), DRM_E_FAIL );

            DRM_BYT_CopyBytes( szExtraXML, 0, argv[3], 0, cchArg );
            DRM_BYT_CopyBytes( szExtraXML, cchArg, pSystemTime, 0, cchSystemTime );
            DRM_BYT_CopyBytes( szExtraXML, cchArg + cchSystemTime, pszXMLEnding, 0, cchXMLEnding);
            TRACE((szExtraXML));
        }
        else
        {
            MEMCPY(szExtraXML, argv[3], min( SIZEOF( szExtraXML ), ( DRM_DWORD ) strlen( argv[3] ) + 1 ) );
        }
    }

    if ( argc > 9 && argv[9] != NULL )
    {
        if( strncmp( argv[9], "NOSYMVAL", 8 ) == 0 )
        {
            fNoSymVal = TRUE;
        }
    }

    if ( argc > 10 && argv[10] != NULL )
    {
        dwNumSlk = atoi( argv[10] );
    }

    if( ( pfUseSlk != NULL) && ( *pfUseSlk ) )
    {
        DRM_CHAR  *pbSlkId   = NULL;
        DRM_CHAR  *pbSlkData = NULL;
        DRM_CHAR  *pbSlk     = NULL;
        DRM_DWORD *pcbSlk    = NULL;
        DRM_DWORD i          = 0;

        DRM_CONST_STRING dstrSlkId   = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSlkData = EMPTY_DRM_STRING;
        DRM_ANSI_STRING dastrSlk     = EMPTY_DRM_STRING;

        MAP_TO_GLOBAL_TEST_VAR( pbSlkId, RESPONSE_V2_SLK_ID );
        MAP_TO_GLOBAL_TEST_VAR( pbSlkData, RESPONSE_V2_SLK_DATA );
        MAP_TO_GLOBAL_TEST_VAR_EX( pbSlk, pcbSlk, RESPONSE_V2_SLK );

        ChkDR( MakeDRMString( &dstrSlkId, pbSlkId) );
        ChkDR( MakeDRMString( &dstrSlkData, pbSlkData) );

        dastrSlk.pszString = pbSlk;
        dastrSlk.cchString = *pcbSlk;

        ChkDR( TestLicGenMakeLicense( atoi( argv[2] ),
                                      argv[3]?szExtraXML:NULL,
                                      NULL,
                                      eLicType,
                                      &pwszLicense,
                                      &dastrSlk,
                                      fNoSymVal) );

        for (i=0; i < dwNumSlk; i++)
        {
            ChkDR( TestResponseAddSLK( pbResponseContext, &dstrSlkId, &dstrSlkData ) );
        }
    }
    else
    {
        ChkDR( TestLicGenMakeLicense( atoi( argv[2] ),
                                      argv[3]?szExtraXML:NULL,
                                      NULL,
                                      eLicType,
                                      &pwszLicense,
                                      NULL,
                                      fNoSymVal) );
    }


    LogBlob( "TestManagerAddLicenseToResponse", "XML License:", (DRM_BYTE*)(pwszLicense.pwszString), pwszLicense.cchString * SIZEOF(DRM_WCHAR) );

    ChkDR( TestResponseAddLicense( pbResponseContext, &pwszLicense ) );


ErrorExit:
    TestLicGenShutDown();
    return dr;
}

/*****************************************************************************
** Function:    TestManagerGenerateResponse
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
DRM_RESULT DRM_CALL TestManagerGenerateResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT   dr                = DRM_SUCCESS;
    _XMBContext *pbResponseContext = NULL;
    DRM_BYTE    *pbSharedResponse  = NULL;
    DRM_BYTE    *pbResponse        = NULL;
    DRM_DWORD    cbResponse        = 0;

    /*
    ** Get shared Response Context if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_CONTEXT_VAR, ( DRM_VOID ** ) &pbResponseContext, NULL ) ) )
    {
        pbResponseContext = NULL;
    }

    /*
    ** get the response
    */
    pbSharedResponse = NULL;

    ChkDR( TestResponseGetResponse( pbResponseContext, &pbResponse, &cbResponse ) );

    /*
    ** Save the response
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbResponse );
    MEMCPY( pbSharedResponse, pbResponse, cbResponse );

ErrorExit:
    SAFE_OEM_FREE(pbResponse);
    return dr;
}

/*****************************************************************************
** Function:    TestManagerParseNonceTokenFromChallenge
**
** Synopsis:
**
** Arguments:
**      argv[0]: Challenge Buffer in the global test data array
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerParseNonceTokenFromChallenge( long argc, __in_ecount(argc) char **argv )
{

    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_BYTE  *pbChallenge       = NULL;
    DRM_DWORD *pcbChallenge      = NULL;

    DRM_ANSI_CONST_STRING g_dastrLicenseChallenge = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData = EMPTY_DRM_SUBSTRING;

    DRM_CHAR        *pszNonce    = NULL;
    DRM_DWORD        cchNonce    = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );

    MAP_TO_GLOBAL_TEST_VAR_EX( pbChallenge, pcbChallenge, argv[0] );

    g_dastrLicenseChallenge.cchString = *pcbChallenge;
    g_dastrLicenseChallenge.pszString = (DRM_CHAR*)pbChallenge;

    /* Check that the challenge is in the correct place..
    **
    **  challenge tag & location:
    **
    ** <xml version>
    **      <soap:Envelope>
    **          <soap:Body>
    **              <AcquireLicense>
    **                  ...
    **                  <challenge>
    **                      xml
    **                  </challenge>
    **              <AcquireLicense>
    **          <soap:Body>
    **      <soap:Envelope>
    ** <xml version>
    **
    **/
    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSOAPBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLicenseNonceTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    MAP_TO_GLOBAL_TEST_VAR( pszNonce, LICENSE_NONCE_VAR );
    DRM_BYT_CopyBytes( pszNonce, 0, g_dastrLicenseChallenge.pszString, dsstrXmlSubData.m_ich, cchNonce );

ErrorExit:

    return dr;

}

/*****************************************************************************
** Function:    TestManagerCreateXMRLicense
**
** Synopsis:
**
** Arguments:
**              argv[0] -- Test Variable Name to hold the license.
**              See Test_CreateXMRLicense for argv[0+n] values.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerCreateXMRLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_BYTE               *pbLicense          = NULL;
    DRM_BYTE               *pbLicenseVar       = NULL;
    DRM_DWORD               cbLicense          = 0;

    ChkArg( argc > 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( Test_CreateXMRLicense( &pbLicense, &cbLicense, TRUE, argc - 1, &argv[1] ) );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbLicenseVar, argv[0], cbLicense );
    MEMCPY(pbLicenseVar, pbLicense, cbLicense );

ErrorExit:
    SAFE_OEM_FREE( pbLicense );
    return dr;
}

/*****************************************************************************
** Function:    TstGetClientECCPubKey
**
** Synopsis:    Returns the client's ECC public key
**
** Arguments:
**              f_cbPubKey -- length of name of f_pbPubKey
**              f_pbPubKey -- buffer to put the public key in
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT TstGetClientECCPubKey( __in DRM_DWORD f_cbPubKey, __out_bcount(f_cbPubKey) DRM_BYTE *f_pbPubKey )
{
    DRM_RESULT                dr          = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pAppContext = NULL;

    ChkArg( f_cbPubKey == ECC_P256_POINT_SIZE_IN_BYTES );
    ChkArg( f_pbPubKey != NULL );

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkArg( pAppContext != NULL );

    memcpy( f_pbPubKey, &pAppContext->oBlackBoxContext.cachedBCertValues.DeviceKeyEncrypt.PublicKey, f_cbPubKey );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TstGetLicenseRightsFromPool
**
** Synopsis:    Gets a list of license rights from a pool file
**
** Arguments:
**              f_pszLicenseRightPoolName -- name of the license rights pool file
**              f_pszLicenseRightsId      -- ID of the license rights data blob
**              f_ppLicenseRights         -- returns a linked list of license right objects.  Caller frees using FreeLicenseRights
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT TstGetLicenseRightsFromPool( __in  const DRM_CHAR  *f_pszLicenseRightPoolName,
                                        __in  const DRM_CHAR  *f_pszLicenseRightsId,
                                        __out LICENSERIGHTS  **f_ppLicenseRights )
{
    DRM_RESULT        dr                     = DRM_SUCCESS;

    DRM_CONST_STRING  RightsPool             = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrPool               = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrPoolID             = EMPTY_DRM_STRING;
    DRM_DWORD         cbRightsPool           = 0;
    DRM_BYTE         *pbRightsPool           = NULL;
    DRM_BYTE         *pbPoolStart            = NULL;
    DRM_BYTE         *pbNullTerminatedRights = NULL;

    ChkArg( f_pszLicenseRightPoolName != NULL );
    ChkArg( f_pszLicenseRightsId != NULL );
    ChkArg( f_ppLicenseRights != NULL );

    /*get the license rights configuration*/
    ChkBOOL( LoadTestFile( "files", f_pszLicenseRightPoolName, &pbRightsPool, &cbRightsPool ) || LoadTestFile( NULL, f_pszLicenseRightPoolName, &pbRightsPool, &cbRightsPool ), DRM_E_FILENOTFOUND );

    ChkDR( MakeDRMString(&dstrPoolID, f_pszLicenseRightsId) );
    ChkMem( dstrPoolID.pwszString );

    /* NULL terminate the rightspool buffer */
    ChkMem( pbNullTerminatedRights = ( DRM_BYTE * )Oem_MemAlloc( cbRightsPool + SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( pbNullTerminatedRights, cbRightsPool + SIZEOF( DRM_WCHAR ) );
    memcpy( pbNullTerminatedRights, pbRightsPool, cbRightsPool );

    if ( pbNullTerminatedRights[ 0 ] == 0xFF && pbNullTerminatedRights[ 1 ] == 0xFE )
    {
        pbPoolStart = pbNullTerminatedRights + 2; // skip over unix format header
    }
    else
    {
        pbPoolStart = pbNullTerminatedRights;
    }

    if( pbPoolStart[ 1 ] != 0 )
    {   // ascii
        ChkDR( MakeDRMString( &dstrPool,  ( DRM_CHAR * )pbPoolStart ) );
        ChkMem( dstrPool.pwszString );
        RightsPool.pwszString = dstrPool.pwszString;
        RightsPool.cchString = dstrPool.cchString;
    }
    else /* wide character */
    {
        RightsPool.pwszString = ( DRM_WCHAR* )pbPoolStart;
        RightsPool.cchString = (DRM_DWORD)DRMCRT_wcslen( RightsPool.pwszString );
    }

    ChkDR( GetLicenseRights( ( DRM_WCHAR* )RightsPool.pwszString, dstrPoolID.pwszString, f_ppLicenseRights ) );

ErrorExit:
    SAFE_OEM_FREE( pbRightsPool );
    SAFE_OEM_FREE( pbNullTerminatedRights );

    FREEDRMSTRING( dstrPool );
    FREEDRMSTRING( dstrPoolID );
    return dr;
}


/*****************************************************************************
** Function:    TstCreateXMRLicenseFromPool
**
** Synopsis:    Creates one or more licenses using data from a pool of license rights.
**
** Arguments:
**              f_pszLicenseRightPoolName -- name of the license rights pool file
**              f_pszLicenseRightsId      -- ID of the license rights data blob
**              f_pOptionalParams         -- optional parameters
**              f_ppList                  -- returns a linked list of license blobs.  Caller frees using TestFreeXMRLicenseList
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT TstCreateXMRLicenseFromPool( __in  const DRM_CHAR                       *f_pszLicenseRightPoolName,
                                        __in  const DRM_CHAR                       *f_pszLicenseRightsId,
                                        __in_opt  OptionalTestMakeXMRLicenseParams *f_pOptionalParams,
                                        __out XMRLICENSELINKLIST                  **f_ppList )
{
    DRM_RESULT        dr                     = DRM_SUCCESS;

    LICENSERIGHTS    *pLicenseRights         = NULL;
    DRM_BYTE         *pbECCPubKey            = NULL;
    DRM_DWORD         cbECCPubKey            = 0;

    ChkArg( f_pszLicenseRightPoolName != NULL );
    ChkArg( f_pszLicenseRightsId != NULL );
    ChkArg( f_ppList != NULL );

    ChkDR( TstGetLicenseRightsFromPool( f_pszLicenseRightPoolName, f_pszLicenseRightsId, &pLicenseRights ) );

    if( f_pOptionalParams && f_pOptionalParams->pbPubKeyOverride != NULL )
    {
        ChkDR( tSetECCPubKey( f_pOptionalParams->pbPubKeyOverride, f_pOptionalParams->cbPubKeyOverride ) );
    }
    else
    {
        ChkMem( pbECCPubKey = ( DRM_BYTE * )Oem_MemAlloc( ECC_P256_POINT_SIZE_IN_BYTES ) );
        cbECCPubKey = ECC_P256_POINT_SIZE_IN_BYTES;
        ChkDR( TstGetClientECCPubKey( cbECCPubKey, pbECCPubKey ) );
        ChkDR( tSetECCPubKey( pbECCPubKey, cbECCPubKey ) );
    }
    ChkDR( TestMakeXMRLicense( 0, pLicenseRights, f_ppList, f_pOptionalParams ) );

ErrorExit:

    SAFE_OEM_FREE( pbECCPubKey );

    FreeLicenseRights( pLicenseRights );

    return dr;
}

/*****************************************************************************
** Function:    TestManagerCreateXMRLicenseFromPool
**
** Synopsis:
**
** Arguments:   argv[0] -- Test Variable Name to hold the license.
**              argv[1] -- Rights Pool File Name
**              argv[2] -- ID within the rights pool
**              argv[3] -- [optional] Domain AccountID GUID, B64 encoded
**              argv[4] -- [optional] Store the content key in this variable (defaults to AES_KEY_VAR)
**
** Returns:     DRM_SUCCESS - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerCreateXMRLicenseFromPool( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                        dr                                    = DRM_SUCCESS;
    DRM_BYTE                         *pbLicenseVar                          = NULL;
    XMRLICENSELINKLIST               *pList                                 = 0;
    OptionalTestMakeXMRLicenseParams *pParams                               = NULL;
    DRM_CONST_STRING                  dstrAccountID                         = EMPTY_DRM_STRING;
    DRM_DWORD                         cbSize                                = SIZEOF( DRM_GUID );
    DRM_BYTE                         *pbCert                                = NULL;
    DRM_DWORD                         cbCert                                = 0;
    PUBKEY_P256                       Pubkey                                = {0};
    DRM_DOMAIN_ID                     DomainID                              = {0};
    DRM_APP_CONTEXT                  *poMgrContext                          = NULL;
    DRM_BYTE                          rgbContentKey[DRM_AES_KEYSIZE_128]    = {0};
    DRM_BYTE                         *prgbSharedKey                         = NULL;

    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkMem( pParams = ( OptionalTestMakeXMRLicenseParams * )Oem_MemAlloc( SIZEOF( OptionalTestMakeXMRLicenseParams ) ) );
    ZEROMEM( pParams, SIZEOF( OptionalTestMakeXMRLicenseParams ) );

    /*
    ** Store the start time of the test case in the optional param struct.
    ** This will be used as a basis for relative removal dates.
    */
    MAP_TO_GLOBAL_TEST_VAR( pParams->pTestStartTime, LICENSE_REMOVAL_DATE_VAR );

    /*
    **  Set the Domain stuff
    */
    if ( argc >= 4 && argv[3] != NULL )
    {
        /*
        **  Set the Domain account ID GUID in pParams
        */
        ChkDR( MakeDRMString( &dstrAccountID, argv[3] ) );
        ChkDR( DRM_B64_DecodeW( &dstrAccountID, &cbSize, (DRM_BYTE*)&DomainID.m_oAccountID, 0 ) );
        pParams->pguidDomainAccountID = &DomainID.m_oAccountID;

        /*
        **  Set the Domain revision in pParams (must not be zero!)
        */
        pParams->dwDomainRevision = 1168300800;

        /*
        **  Set the public key (from the domain certificate) in pParams
        */
        GET_SHARED_APP_CONTEXT( poMgrContext );

        dr = Drm_DomainCert_Find( poMgrContext, &DomainID, NULL, &cbCert );

        /* Allocate memory for the cert */
        ChkArg( dr == DRM_E_BUFFERTOOSMALL );
        ChkMem( pbCert = (DRM_BYTE *)Oem_MemAlloc( max( 1, cbCert ) ) );

        ChkDR( Drm_DomainCert_Find( poMgrContext, &DomainID, pbCert, &cbCert ) );

        ChkDR( DRM_BCert_GetPublicKey( pbCert, cbCert, 0, &Pubkey ) );
        pParams->pbPubKeyOverride = (DRM_BYTE *)&Pubkey;
        pParams->cbPubKeyOverride = SIZEOF( PUBKEY_P256 );
    }

    pParams->pbContentKeyUsed = rgbContentKey;
    pParams->cbContentKeyUsed = SIZEOF(rgbContentKey);

    ChkDR( TstCreateXMRLicenseFromPool( argv[1], argv[2], pParams, &pList ) );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbSharedKey, ( argc >= 5 && argv[4] != NULL ) ? argv[4] : AES_KEY_VAR, pParams->cbContentKeyUsed );
    MEMCPY( prgbSharedKey, rgbContentKey, pParams->cbContentKeyUsed );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbLicenseVar, argv[0], pList->cbLicense );
    MEMCPY(pbLicenseVar, pList->pLicense, pList->cbLicense );

ErrorExit:
    TestFreeXMRLicenseList( pList );    
    SAFE_OEM_FREE( pbCert );
    FREEDRMSTRING( dstrAccountID );
    FreeOptionalTestMakeXMRLicenseParams( pParams );
    SAFE_OEM_FREE( pParams );
    
    return dr;
}


/*****************************************************************************
** Function:    TestManagerAddXMRLicense
**
** Synopsis:
**
** Arguments:   See Test_CreateXMRLicense for argv values.
**              argv[38] - Optional, custom data
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddXMRLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_BYTE                 *pbLicense          = NULL;
    DRM_DWORD                 cbLicense          = 0;
    DRM_BYTE                 *pbSharedResponse   = NULL;
    DRM_DWORD                 cbSharedResponse   = 0;
    DRM_DWORD                 cLicenses          = 1;
    DRM_ANSI_CONST_STRING     rgdastrLicenses[ 1 ] = { EMPTY_DRM_STRING };
    DRM_ANSI_CONST_STRING     dastrTransactionID = CREATE_DRM_ANSI_STRING( "31e898e3-37a0-4b47-ac94-7518418d0174" );
    DRM_DWORD                 cbBuffer           = 0;
    DRM_BYTE                 *pbBuffer           = NULL;
    DRM_APP_CONTEXT_INTERNAL *pAppContext        = NULL;
    DRM_ANSI_CONST_STRING     dastrCustomData    = EMPTY_DRM_STRING;
    DRM_STRING               *pRevocationInfo    = NULL;
    DRM_DWORD                 cbRevInfo          = 0;
    DRM_CHAR                 *pbRevInfo          = NULL;
    DRM_ANSI_CONST_STRING     dastrRevInfo       = EMPTY_DRM_STRING;

    ChkDR( Test_CreateXMRLicense( &pbLicense, &cbLicense, TRUE, argc, argv ) );

    GET_SHARED_APP_CONTEXT( pAppContext );

    dr = DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, NULL, &cbBuffer, 0 );
    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }

    ChkMem( pbBuffer = (DRM_BYTE*)Oem_MemAlloc( cbBuffer + 2 ) );
    ZEROMEM( pbBuffer, cbBuffer+2 );

    ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, (DRM_CHAR*)pbBuffer, &cbBuffer, 0 ) );
    pbBuffer[cbBuffer] = 0;

    if( argc > 38 && argv[38] != NULL )
    {
        dastrCustomData.pszString = argv[38];
        dastrCustomData.cchString = (DRM_DWORD)strlen(argv[38]);
    }

    DASTR_FROM_PB( &rgdastrLicenses[ 0 ], pbBuffer, cbBuffer );

    /*
    ** If REV_INFO_VAR is not NULL, and the Length is not 0, we need to add revocation data to the response:
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( REV_INFO_VAR, ( DRM_VOID ** ) &pRevocationInfo, NULL ) ) )
    {
        pRevocationInfo = NULL;
    }

    if( pRevocationInfo != NULL && pRevocationInfo->pwszString != NULL && pRevocationInfo->cchString > 0)
    {
        cbRevInfo = 1 + pRevocationInfo->cchString;
        ChkMem(pbRevInfo = (DRM_CHAR *)Oem_MemAlloc(cbRevInfo));
        ZEROMEM( pbRevInfo, cbRevInfo );
        DRM_UTL_DemoteUNICODEtoASCII((const DRM_WCHAR *)pRevocationInfo->pwszString,(DRM_CHAR *) pbRevInfo, pRevocationInfo->cchString);
        DASTR_FROM_PB( &dastrRevInfo, pbRevInfo, cbRevInfo );
    }

    dr = Test_BuildV3LicenseResponse(
                                  &pAppContext->oBlackBoxContext,
                                  rgdastrLicenses,
                                  cLicenses,
                                  dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                  dastrCustomData.cchString > 0 ? &dastrCustomData : NULL,
                                  &dastrTransactionID,
                                  NULL,
                                  &cbSharedResponse );

    ChkBOOL ( dr == DRM_E_BUFFERTOOSMALL, DRM_E_FAIL );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbSharedResponse );

    ChkDR( Test_BuildV3LicenseResponse(
                                    &pAppContext->oBlackBoxContext,
                                    rgdastrLicenses,
                                    cLicenses,
                                    dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                    dastrCustomData.cchString > 0 ? &dastrCustomData : NULL,
                                    &dastrTransactionID,
                                    pbSharedResponse,
                                    &cbSharedResponse ) );


ErrorExit:
    SAFE_OEM_FREE( pbLicense );
    SAFE_OEM_FREE( pbBuffer  );
    SAFE_OEM_FREE( pbRevInfo  );
    return dr;
}


/*****************************************************************************
** Function:    TestManagerAddXMRLicenseFromFile
**
** Synopsis:
**
** Arguments:   argv[0] filename of XMR v3 license to add
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddXMRLicenseFromFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_BYTE                 *pbLicense          = NULL;
    DRM_DWORD                 cbLicense          = 0;
    DRM_BYTE                 *pbSharedResponse   = NULL;
    DRM_DWORD                 cbSharedResponse   = 0;
    DRM_DWORD                 cLicenses          = 1;
    DRM_ANSI_CONST_STRING     rgdastrLicenses[ 1 ] = { EMPTY_DRM_STRING };
    DRM_ANSI_CONST_STRING     dastrTransactionID = CREATE_DRM_ANSI_STRING( "31e898e3-37a0-4b47-ac94-7518418d0174" );
    DRM_DWORD                 cbBuffer           = 0;
    DRM_BYTE                 *pbBuffer           = NULL;
    DRM_APP_CONTEXT_INTERNAL *pAppContext        = NULL;
    DRM_ANSI_CONST_STRING     dastrCustomData    = EMPTY_DRM_STRING;
    DRM_STRING               *pRevocationInfo    = NULL;
    DRM_DWORD                 cbRevInfo          = 0;
    DRM_CHAR                 *pbRevInfo          = NULL;
    DRM_ANSI_CONST_STRING     dastrRevInfo       = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( "files", argv[0], &pbLicense, &cbLicense )
          || LoadTestFile( NULL, argv[0], &pbLicense, &cbLicense ), DRM_E_TEST_INCOMPLETE );

    GET_SHARED_APP_CONTEXT( pAppContext );

    dr = DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, NULL, &cbBuffer, 0 );
    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }

    ChkMem( pbBuffer = (DRM_BYTE*)Oem_MemAlloc( cbBuffer + 2 ) );
    ZEROMEM( pbBuffer, cbBuffer+2 );

    ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, (DRM_CHAR*)pbBuffer, &cbBuffer, 0 ) );
    pbBuffer[cbBuffer] = 0;

    if( argc >= 35 && argv[34] != NULL )
    {
        dastrCustomData.pszString = argv[34];
        dastrCustomData.cchString = (DRM_DWORD)strlen(argv[34]);
    }

    DASTR_FROM_PB( &rgdastrLicenses[ 0 ], pbBuffer, cbBuffer );

    /*
    ** If REV_INFO_VAR is not NULL, and the Length is not 0, we need to add revocation data to the response:
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( REV_INFO_VAR, ( DRM_VOID ** ) &pRevocationInfo, NULL ) ) )
    {
        pRevocationInfo = NULL;
    }

    if( pRevocationInfo != NULL && pRevocationInfo->pwszString != NULL && pRevocationInfo->cchString > 0)
    {
        cbRevInfo = 1 + pRevocationInfo->cchString;
        ChkMem(pbRevInfo = (DRM_CHAR *)Oem_MemAlloc(cbRevInfo));
        ZEROMEM( pbRevInfo, cbRevInfo );
        DRM_UTL_DemoteUNICODEtoASCII((const DRM_WCHAR *)pRevocationInfo->pwszString,(DRM_CHAR *) pbRevInfo, pRevocationInfo->cchString);
        DASTR_FROM_PB( &dastrRevInfo, pbRevInfo, cbRevInfo );
    }

    dr = Test_BuildV3LicenseResponse(
                                  &pAppContext->oBlackBoxContext,
                                  rgdastrLicenses,
                                  cLicenses,
                                  dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                  dastrCustomData.cchString > 0 ? &dastrCustomData : NULL,
                                  &dastrTransactionID,
                                  NULL,
                                  &cbSharedResponse );

    ChkBOOL ( dr == DRM_E_BUFFERTOOSMALL, DRM_E_FAIL );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbSharedResponse );

    ChkDR( Test_BuildV3LicenseResponse(
                                    &pAppContext->oBlackBoxContext,
                                    rgdastrLicenses,
                                    cLicenses,
                                    dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                    dastrCustomData.cchString > 0 ? &dastrCustomData : NULL,
                                    &dastrTransactionID,
                                    pbSharedResponse,
                                    &cbSharedResponse ) );


ErrorExit:
    SAFE_OEM_FREE( pbLicense );
    SAFE_OEM_FREE( pbBuffer  );
    SAFE_OEM_FREE( pbRevInfo  );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerGenerateXMRResponse
**
** Synopsis:
**
** Arguments:   argv[n] : TestVariable holding the license data (max of 30)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGenerateXMRResponse( long argc, __in_ecount( argc ) char** argv )
{
#define DRM_TEST_LICGEN_MAX_LICENSES 30

    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_BYTE                 *pbSharedResponse   = NULL;
    DRM_DWORD                 cbSharedResponse   = 0;
    DRM_ANSI_CONST_STRING     rgdastrLicenses[ DRM_TEST_LICGEN_MAX_LICENSES ] = { EMPTY_DRM_STRING };
    DRM_LONG                  iLicense           = 0;
    DRM_ANSI_CONST_STRING     dastrTransactionID = CREATE_DRM_ANSI_STRING( "31e898e3-37a0-4b47-ac94-7518418d0174" );
    DRM_APP_CONTEXT_INTERNAL *pAppContext        = NULL;
    DRM_STRING               *pRevocationInfo    = NULL;
    DRM_DWORD                 cbRevInfo          = 0;
    DRM_CHAR                 *pbRevInfo          = NULL;
    DRM_ANSI_CONST_STRING     dastrRevInfo       = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argc <= DRM_TEST_LICGEN_MAX_LICENSES );
    ChkArg( NULL != argv[0] );

    GET_SHARED_APP_CONTEXT( pAppContext );

    for( iLicense = 0; iLicense < argc; iLicense++ )
    {
        DRM_BYTE  *pbLicense  = NULL;
        DRM_DWORD *pcbLicense = NULL;
        DRM_DWORD  cbBuffer   = 0;
        DRM_BYTE  *pbBuffer   = NULL;

        MAP_TO_GLOBAL_TEST_VAR_EX( pbLicense, pcbLicense, argv[iLicense] );

        dr = DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, *pcbLicense, NULL, &cbBuffer, 0 );
        if (dr != DRM_E_BUFFERTOOSMALL)
        {
            ChkDR(dr);
        }

        ChkMem( pbBuffer = (DRM_BYTE*)Oem_MemAlloc( cbBuffer + 2 ) );
        ZEROMEM( pbBuffer, cbBuffer+2 );

        ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, *pcbLicense, (DRM_CHAR*)pbBuffer, &cbBuffer, 0 ) );
        pbBuffer[cbBuffer] = 0;

        DASTR_FROM_PB( &rgdastrLicenses[ iLicense ], pbBuffer, cbBuffer );

    }

    /*
    ** If REV_INFO_VAR is not NULL, and the Length is not 0, we need to add revocation data to the response:
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( REV_INFO_VAR, ( DRM_VOID ** ) &pRevocationInfo, NULL ) ) )
    {
        pRevocationInfo = NULL;
    }

    if( pRevocationInfo != NULL && pRevocationInfo->pwszString != NULL && pRevocationInfo->cchString > 0)
    {
        cbRevInfo = 1 + pRevocationInfo->cchString;
        ChkMem(pbRevInfo = (DRM_CHAR *)Oem_MemAlloc(cbRevInfo));
        ZEROMEM( pbRevInfo, cbRevInfo );
        DRM_UTL_DemoteUNICODEtoASCII((const DRM_WCHAR *)pRevocationInfo->pwszString,(DRM_CHAR *) pbRevInfo, pRevocationInfo->cchString);
        DASTR_FROM_PB( &dastrRevInfo, pbRevInfo, cbRevInfo );
    }

    dr = Test_BuildV3LicenseResponse(
                                  &pAppContext->oBlackBoxContext,
                                  rgdastrLicenses,
                                  argc,
                                  dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                  NULL,
                                  &dastrTransactionID,
                                  NULL,
                                  &cbSharedResponse );

    ChkBOOL ( dr == DRM_E_BUFFERTOOSMALL, DRM_E_FAIL );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbSharedResponse );

    ChkDR( Test_BuildV3LicenseResponse(
                                    &pAppContext->oBlackBoxContext,
                                    rgdastrLicenses,
                                    argc,
                                    dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                    NULL,
                                    &dastrTransactionID,
                                    pbSharedResponse,
                                    &cbSharedResponse ) );

ErrorExit:
    for( iLicense = 0; iLicense < DRM_TEST_LICGEN_MAX_LICENSES ; iLicense++ )
    {
        SAFE_OEM_FREE( rgdastrLicenses[ iLicense ].pszString );
    }
    SAFE_OEM_FREE( pbRevInfo  );
    return dr;
#undef DRM_TEST_LICGEN_MAX_LICENSES

}

/*****************************************************************************
** Function:    _AddManyXMRLicenses
**
** Synopsis:
**
** Arguments:   f_fUsePool - if TRUE create licenses using TestMakeXMRLicense instead of Test_CreateXMRLicense
**              argv[0] : number of licenses to add
**              argv[1] : if f_fUsePool is TRUE then the file name of the pool else the KID
**              argv[2] : if f_fUsePool is TRUE then the id with in the pool file else the LID
**              argv[3] : key seed (ignored if f_fUsePool is TRUE)
**              argv[4] : optional. play count  (ignored if f_fUsePool is TRUE)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL _AddManyXMRLicenses( __in DRM_BOOL f_fUsePool, __in long argc, __in_ecount( argc ) char **argv )
{
#define DRM_TEST_MAX_LICENSES_PER_RESPONSE 20
    DRM_RESULT            dr                                                              = DRM_SUCCESS;
    DRM_DWORD             iLicense                                                        = 0;
    DRM_DWORD             dwLicenses                                                      = 0;
    DRM_LICENSE_RESPONSE  oJunk                                                           = { eUnknownProtocol, 0 };
    DRM_APP_CONTEXT      *pAppContext                                                     = NULL;
    DRM_BYTE             *pbResponse                                                      = NULL;
    DRM_DWORD             cbResponse                                                      = 0;
    DRM_ANSI_CONST_STRING dastrTransactionID                                              = CREATE_DRM_ANSI_STRING( "31e898e3-37a0-4b47-ac94-7518418d0174" );
    DRM_DWORD             cbBuffer                                                        = 0;
    DRM_BYTE             *pbBuffer                                                        = NULL;
    DRM_BYTE             *pbLicense                                                       = NULL;
    DRM_DWORD             cbLicense                                                       = 0;
    DRM_STRING           *pRevocationInfo                                                 = NULL;
    DRM_DWORD             cbRevInfo                                                       = 0;
    DRM_CHAR*             pbRevInfo                                                       = NULL;
    DRM_ANSI_CONST_STRING dastrRevInfo                                                    = EMPTY_DRM_STRING;

    DRM_ANSI_CONST_STRING rgdastrLicenses[ DRM_TEST_MAX_LICENSES_PER_RESPONSE ]           = { EMPTY_DRM_STRING };
    DRM_DWORD             cLicenses                                                       = 0;

    LICENSERIGHTS        *pLicenseRights                                                  = NULL;
    DRM_BYTE             *pbECCPubKey                                                     = NULL;
    DRM_DWORD             cbECCPubKey                                                     = 0;
    XMRLICENSELINKLIST   *pList                                                           = 0;
    OptionalTestMakeXMRLicenseParams *pParams                                             = NULL;

    DRM_DWORD             dwKIDListIndex                                                  = 0;
    ResponseKIDListData  *poKIDListData                                                   = NULL;
    DRM_DWORD            *pdwKIDListIndex                                                 = NULL;
    DRM_BYTE              rgbContentKeyUsed[ DRM_AES_KEYSIZE_128 ]                        = { 0 };
#if DBG
    DRM_DWORD             cchEncoded                                                      = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 );
    DRM_CHAR              rgcB64ContentKey[ CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 ) + 1 ] = { 0 };
#endif
    

    if( f_fUsePool )
    {
        ChkArg( argc >= 3 );
    }
    else
    {
        ChkArg( argc > 3 );
    }

    ChkArg( argv[0] != NULL );

    GET_SHARED_APP_CONTEXT( pAppContext );

    dwLicenses = atol( argv[0] );
    ChkArg( dwLicenses > 0 );

    Log( "Trace", "Adding %d XMR Licenses", dwLicenses );

    if( f_fUsePool )
    {
        ChkArg( argv[1] != NULL );
        ChkArg( argv[2] != NULL );

        ChkMem( pParams = ( OptionalTestMakeXMRLicenseParams * )Oem_MemAlloc( SIZEOF( OptionalTestMakeXMRLicenseParams ) ) );
        ZEROMEM( pParams, SIZEOF( OptionalTestMakeXMRLicenseParams ) );

        ChkDR( TstGetLicenseRightsFromPool( argv[1], argv[2], &pLicenseRights ) );

        ChkMem( pbECCPubKey = ( DRM_BYTE * )Oem_MemAlloc( ECC_P256_POINT_SIZE_IN_BYTES ) );
        cbECCPubKey = ECC_P256_POINT_SIZE_IN_BYTES;
        ChkDR( TstGetClientECCPubKey( cbECCPubKey, pbECCPubKey ) );
        ChkDR( tSetECCPubKey( pbECCPubKey, cbECCPubKey ) );

        /*
        ** Store the start time of the test case in the optional param struct.
        ** This will be used as a basis for relative removal dates.
        */
        MAP_TO_GLOBAL_TEST_VAR( pParams->pTestStartTime, LICENSE_REMOVAL_DATE_VAR );

        /*
        ** Allocate and initialize the variables for recording all the unique KIDs and Content Keys
        ** of the generated licenses so that we can test binding to each/any of these licenses later.
        */
        MAP_TO_GLOBAL_TEST_VAR_Alloc( poKIDListData, RESPONSE_KIDLIST_DATA,  ( dwLicenses * SIZEOF( ResponseKIDListData ) ) );   
        ZEROMEM( poKIDListData, ( dwLicenses * SIZEOF( ResponseKIDListData ) ) );
        
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwKIDListIndex, RESPONSE_KIDLIST_INDEX,  SIZEOF( DRM_DWORD ) );   
        *pdwKIDListIndex = 0;
        
        /*
        ** Make room in the optional parameters structure (a parameter to the TestMakeXMRLicense
        ** function below) so that that function will return to us the content key used.
        */
        pParams->pbContentKeyUsed = (DRM_BYTE *)&rgbContentKeyUsed;
        pParams->cbContentKeyUsed = SIZEOF( rgbContentKeyUsed );
    }

    for( iLicense = 0; iLicense < dwLicenses; iLicense++ )
    {
        if( f_fUsePool )
        {
            TestFreeXMRLicenseList( pList );
            pList = NULL;
            ChkDR( TestMakeXMRLicense( 0, pLicenseRights, &pList, pParams ) );
            pbLicense = pList->pLicense;
            pList->pLicense = NULL;
            cbLicense = pList->cbLicense;
            pList->cbLicense = 0;

            /*
            ** Remember the KID and Content Key
            */
#if DBG
            Log( "Trace", "\t KID         %d = %S", ( dwKIDListIndex + 1 ), pParams->dstrKIDUsed.pwszString );
            ChkDR( DRM_B64_EncodeA( pParams->pbContentKeyUsed,
                                    pParams->cbContentKeyUsed,
                                    rgcB64ContentKey,
                                   &cchEncoded,
                                    0 ) );
            Log( "Trace", "\t Content Key %d = %s", ( dwKIDListIndex + 1 ), rgcB64ContentKey );
#endif
                
            MEMCPY( (DRM_BYTE *)&( poKIDListData[ dwKIDListIndex ].rgwchKID ), 
                    pParams->dstrKIDUsed.pwszString,
                    DRM_ID_B64_WCHAR_LEN );

            MEMCPY( poKIDListData[ dwKIDListIndex ].rgbContentKey, 
                    pParams->pbContentKeyUsed,
                    pParams->cbContentKeyUsed );

            dwKIDListIndex++;
        }
        else
        {
            ChkDR( Test_CreateXMRLicense(
                                      &pbLicense,
                                      &cbLicense,
                                      iLicense == 0, /* Only write log information for the first license */
                                      argc - 1,      /* Arguments after argv[0] determine the license to make */
                                      &argv[1] ) );
        }

        dr = DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, NULL, &cbBuffer, 0 );
        if (dr != DRM_E_BUFFERTOOSMALL)
        {
            ChkDR(dr);
        }

        ChkMem( pbBuffer = (DRM_BYTE*)Oem_MemAlloc( cbBuffer + 2 ) );
        ZEROMEM( pbBuffer, cbBuffer+2 );

        ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)pbLicense, cbLicense, (DRM_CHAR*)pbBuffer, &cbBuffer, 0 ) );
        pbBuffer[cbBuffer] = 0;

        ChkBOOL( cLicenses < DRM_TEST_MAX_LICENSES_PER_RESPONSE, DRM_E_TEST_UNEXPECTED_OUTPUT );

        SAFE_OEM_FREE( rgdastrLicenses[ cLicenses ].pszString );
        DASTR_FROM_PB( &rgdastrLicenses[ cLicenses ], pbBuffer, cbBuffer );
        cLicenses++;

        if ( cLicenses    == DRM_TEST_MAX_LICENSES_PER_RESPONSE
          || iLicense + 1 == dwLicenses )
        {
            Log( "Trace", "\t Building and Processing a Response with %d Licenses", cLicenses );

            /*
            ** If REV_INFO_VAR is not NULL, and the Length is not 0, we need to add revocation data to the response:
            */
            if ( DRM_FAILED( DRM_TST_GetTestVar( REV_INFO_VAR, ( DRM_VOID ** ) &pRevocationInfo, NULL ) ) )
            {
                pRevocationInfo = NULL;
            }

            if( pRevocationInfo != NULL && pRevocationInfo->pwszString != NULL && pRevocationInfo->cchString > 0)
            {
                cbRevInfo = 1 + pRevocationInfo->cchString;
                ChkMem(pbRevInfo = (DRM_CHAR *)Oem_MemAlloc(cbRevInfo));
                ZEROMEM( pbRevInfo, cbRevInfo );
                DRM_UTL_DemoteUNICODEtoASCII((const DRM_WCHAR *)pRevocationInfo->pwszString,(DRM_CHAR *) pbRevInfo, pRevocationInfo->cchString);
                DASTR_FROM_PB( &dastrRevInfo, pbRevInfo, cbRevInfo );
            }

            dr = Test_BuildV3LicenseResponse(
                                          &((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->oBlackBoxContext,
                                           rgdastrLicenses,
                                           cLicenses,
                                           dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                           NULL,
                                          &dastrTransactionID,
                                           NULL,
                                          &cbResponse );

            ChkBOOL ( dr == DRM_E_BUFFERTOOSMALL, DRM_E_FAIL );

            SAFE_OEM_FREE( pbResponse );
            ChkMem( pbResponse = ( DRM_BYTE * ) Oem_MemAlloc( cbResponse ) );
            ZEROMEM( pbResponse, cbResponse );

            ChkDR( Test_BuildV3LicenseResponse(
                                            &((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->oBlackBoxContext,
                                             rgdastrLicenses,
                                             cLicenses,
                                             dastrRevInfo.cchString>0? &dastrRevInfo: NULL,
                                             NULL,
                                            &dastrTransactionID,
                                             pbResponse,
                                            &cbResponse ) );



            dr = Drm_LicenseAcq_ProcessResponse( pAppContext,
                                                 DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                                 NULL,
                                                 NULL,
                                                 pbResponse,
                                                 cbResponse,
                                                &oJunk );
            if ( dr != DRM_S_MORE_DATA )
            {
                ChkDR(dr);
            }
            dr = DRM_SUCCESS;
            cLicenses = 0;
        }
    }

ErrorExit:
    for( iLicense = 0; iLicense < DRM_TEST_MAX_LICENSES_PER_RESPONSE; iLicense++ )
    {
        SAFE_OEM_FREE( rgdastrLicenses[ iLicense ].pszString );
    }
    SAFE_OEM_FREE( pbResponse );
    SAFE_OEM_FREE( pbRevInfo  );

    SAFE_OEM_FREE( pbECCPubKey );

    FreeLicenseRights( pLicenseRights );

    TestFreeXMRLicenseList( pList );

    FreeOptionalTestMakeXMRLicenseParams( pParams );        
    SAFE_OEM_FREE( pParams );
    
    return dr;
}

/*****************************************************************************
** Function:    TestManagerAddManyXMRLicenses
**
** Synopsis:
**
** Arguments:   argv[0] : number of licenses to add
**              argv[1] : KID
**              argv[2] : LID
**              argv[3] : key seed
**              argv[4] : optional. play count
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddManyXMRLicenses( long argc, __in_ecount( argc ) char **argv )
{
    return( _AddManyXMRLicenses( FALSE, argc, argv ) );
}

/*****************************************************************************
** Function:    TestManagerAddManyXMRLicensesFromPool
**
** Synopsis:
**
** Arguments:   argv[0] : number of licenses to add
**              argv[1] : License rights pool file name
**              argv[2] : ID within the rights pool file
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddManyXMRLicensesFromPool( long argc, __in_ecount( argc ) char **argv )
{
    return( _AddManyXMRLicenses( TRUE, argc, argv ) );
}

/*****************************************************************************
** Function:    TestManagerAddManyLicenses
**
** Synopsis:
**
** Arguments:   argv[0]: number of licenses to add
**              argv[1]: optional, maximum number of total milliseconds processlicenseresponse may take
**              argv[2]: KID
**              argv[3]: LID
**              argv[4]: option in creating a test license
**              argv[5]: extra xml string to add to the license
**              argv[6]: content sign key
**              argv[7]: key seed
**              argv[8]: optional license type: 0(simple), 1(leaf), 2(root)
**              argv[9]: optional uplink kid
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddManyLicenses( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  iLicense = 0;
    DRM_DWORD  dwLicenses = 0;
    DRM_LICENSE_RESPONSE oJunk = { eUnknownProtocol, 0 };
    DRM_CHAR *pchKeepOACRHappy = NULL;
    DRM_APP_CONTEXT      *poManagerContext = NULL;
    DRM_BYTE             *pbSharedResponse = NULL;
    DRM_DWORD             cbSharedResponse = 0;
    DRM_DWORD            *pcbSharedResp    = &cbSharedResponse;
#if DRM_SUPPORT_PROFILING
    DRM_UINT64 ui64ProcessStartTime = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64ProcessTotalTime = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64MaxProcessTime   = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64FirstProcessTime = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64LastProcessTime  = DRM_UI64LITERAL( 0, 0 );
#endif

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    dwLicenses = atol( argv[0] );
    ChkArg( dwLicenses > 0 );

#if DRM_SUPPORT_PROFILING
    if( argc > 1 && argv[1] != NULL )
    {
        ui64MaxProcessTime = DRM_UI64( atol( argv[1] ) );
    }
#endif

    for( iLicense = 0; iLicense < dwLicenses; iLicense++ )
    {
        ChkDR( TestManagerInitResponse( 0, &pchKeepOACRHappy ) );
        ChkDR( TestManagerAddLicenseToResponse( argc - 2, &argv[2] ) );
        ChkDR( TestManagerGenerateResponse( 0, &pchKeepOACRHappy ) );
        /*
        ** Get RESPONSE_VAR
        */
        if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_VAR, ( DRM_VOID ** ) &pbSharedResponse, &pcbSharedResp ) ) )
        {
            pcbSharedResp = &cbSharedResponse;
            pbSharedResponse = NULL;
        }

#if DRM_SUPPORT_PROFILING
        if( DRM_UI64Les( DRM_UI64(0), ui64MaxProcessTime ) )
        {
            START_TIMER( ui64ProcessStartTime );
        }
#endif
        ChkDR( Drm_LicenseAcq_ProcessResponse( poManagerContext,
               DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
               NULL,
               NULL,
               pbSharedResponse,
               *pcbSharedResp,
               &oJunk ) );
#if DRM_SUPPORT_PROFILING
        if( DRM_UI64Les( DRM_UI64(0), ui64MaxProcessTime ) )
        {
            STOP_TIMER( ui64ProcessStartTime, ui64LastProcessTime );
            if( DRM_UI64Eql( DRM_UI64(0), ui64FirstProcessTime ) )
            {
                ui64FirstProcessTime = ui64LastProcessTime;
            }
            ui64ProcessTotalTime = DRM_UI64Add( ui64ProcessTotalTime, ui64LastProcessTime );
        }
#endif
    }

#if DRM_SUPPORT_PROFILING
    if( DRM_UI64Les( DRM_UI64(0), ui64MaxProcessTime ) )
    {
        Log( "Perf",
             "Drm_LicenseAcq_ProcessResponse took %d ms to execute for %d licenses (%f ms per license). First license took %d ms, final took %d ms.",
             ui64ProcessTotalTime,
             dwLicenses,
             (float)DRM_UI64Low32(DRM_UI64Div( ui64ProcessTotalTime, DRM_UI64(dwLicenses) )),
             ui64FirstProcessTime,
             ui64LastProcessTime );

        if( DRM_UI64Les( ui64MaxProcessTime, ui64ProcessTotalTime ) )
        {
            ChkDR( DRM_E_TEST_TOO_SLOW );
        }
    }
#endif

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    LoadResponseFromFile
**
** Synopsis:    Load the license response stream from file
**
** Arguments:   argv[0]: File name (in files directory or working directory)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL LoadResponseFromFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr               = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE   *pbSharedResponse = NULL;
    DRM_DWORD   cbSharedResponse = 0;
    DRM_BYTE   *pbResponse       = NULL;

    ChkArg( argc    == 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( "files", argv[0], &pbResponse, &cbSharedResponse )
          || LoadTestFile( NULL, argv[0], &pbResponse, &cbSharedResponse ), DRM_E_TEST_INCOMPLETE );

    /*
    ** Save the response
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbSharedResponse );
    MEMCPY( pbSharedResponse, pbResponse, cbSharedResponse );

    dr = DRM_SUCCESS;

ErrorExit:
    /*
    ** Free memory allocated by LoadTestFile
    */
    SAFE_OEM_FREE( pbResponse );
    return dr;
}


/*****************************************************************************
** Function:    AddMultipleXMRLicenses
**
** Synopsis:    Adds multiple XMR licenses to device store
**
**
** Arguments:   argv[0]        -- name of the file which contains XMR license responses.
** These files should follow the naming convention <filename><index>.dat. argv[0] needs
** <filename>, and depending on the number of licenses to be added, it will increment
** <index> to access subsequent files containing license responses
**               argv[1]       -- number response files to be processed.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL AddMultipleXMRLicenses(long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_CHAR   chResponseFile[50]  = {0};
    DRM_CHAR   chIndex[5]          = {0};
    DRM_LONG   iTotalResponseFiles = 0;
    DRM_DWORD  dwSourceLen         = 0;
    DRM_DWORD  dwIndex             = 0;
    DRM_DWORD  dwOutputSize        = 0;
    DRM_WCHAR  wIndex[5]           = {0};
    DRM_BYTE  *pbSharedResponse    = NULL;
    DRM_DWORD  cbSharedResponse    = 0;
    DRM_BYTE  *pbResponse          = NULL;
    DRM_APP_CONTEXT      *poManagerContext  = NULL;
    DRM_LICENSE_RESPONSE *poLicenseResponse = NULL;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( poLicenseResponse, LICENSE_RESPONSE_VAR );

    iTotalResponseFiles = atoi( argv[1] );
    ChkArg( iTotalResponseFiles > 0 );

    ChkDR( DRM_STR_StringCchLengthA( argv[0], 50, &dwSourceLen ) );

    for( dwIndex = 0; dwIndex < (DRM_DWORD)iTotalResponseFiles; dwIndex++ )
    {
        ChkDR( DRM_STR_StringCchCopyNA( chResponseFile, 50, argv[0], dwSourceLen ) );
        ChkDR( DRM_UTL_NumberToString( dwIndex+1,wIndex, NO_OF( wIndex ), 0, 10, &dwOutputSize ) );
        DRM_UTL_DemoteUNICODEtoASCII( wIndex, chIndex, NO_OF( wIndex ) );
        ChkDR( DRM_STR_StringCchCatA( chResponseFile, 50, chIndex ) );
        ChkDR( DRM_STR_StringCchCatA( chResponseFile, 50, ".dat" ) );

        pbSharedResponse = NULL;
        cbSharedResponse = 0;

        ChkBOOL( TRUE == (LoadTestFile( "files", chResponseFile, &pbResponse, &cbSharedResponse )
                          || LoadTestFile( NULL, chResponseFile, &pbResponse, &cbSharedResponse )), DRM_E_TEST_INCOMPLETE );

        /*
        ** Save the response
        */
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbSharedResponse, RESPONSE_VAR, cbSharedResponse );
        MEMCPY( pbSharedResponse, pbResponse, cbSharedResponse );

        ChkDR( Drm_LicenseAcq_ProcessResponse(
               poManagerContext,
               DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
               NULL,
               NULL,
               pbSharedResponse,
               cbSharedResponse,
               poLicenseResponse ) );
        ChkDR( poLicenseResponse->m_dwResult );
    }

ErrorExit:
    /*
    ** Free memory allocated by LoadTestFile
    */
    SAFE_OEM_FREE( pbResponse );
    return dr;
}

/*****************************************************************************
** Function:    GenerateRandomHeader
**
** Synopsis:    This function is used to generate an XML License header for a random KID. This random KID is then stored
**              in a global variable to be used for generating the corresponding license response, and the header is stored
**              in a file whose name is provided as the input argument.
**
** Arguments:   argv[0] - KeySeed
**              argv[1] - Signing Key
**              argv[2] - Verify Key
**              argv[3] - LA URL Info
**              argv[4] - Name of the header file to be generated
**              argv[5] - ID specifying whether V2(0), V4(1), V4.1(2) content header is to be generated
**
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL GenerateRandomHeader( long argc, __in_ecount( argc ) char **argv )
{
    #define LIC_RESP_TEST_V2_HEADER   ( 0 )
    #define LIC_RESP_TEST_V4_HEADER   ( 1 )
    #define LIC_RESP_TEST_V4_1_HEADER ( 2 )
    DRM_RESULT               dr               = DRM_SUCCESS;
    DRM_BYTE                *pbContext        = NULL;
    DRM_BYTE                *pbHeaderXML      = NULL;
    DRM_BYTE                 cbContentKeySize = 0;
    DRM_DWORD                cbContext        = 0;
    DRM_DWORD                cbHeaderXML      = 0;
    DRM_DWORD                cbWritten        = 0;
    DRM_DWORD                dwHeaderVersion  = 0;
    DRM_CONST_STRING         dstrKeySeed      = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrSigningKey   = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrVerifyKey    = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrLAUrl        = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrKeyID        = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrHeaderFile   = EMPTY_DRM_STRING;
    DRM_STRING               dstrContentKey   = EMPTY_DRM_STRING;
    DRM_SUBSTRING            dasstr           = {0};
    DRM_CRYPTO_CONTEXT      *pCrypto          = NULL;
    OEM_FILEHDL              fp               = OEM_INVALID_HANDLE_VALUE;
    DRM_CONTENT_SET_PROPERTY eHeaderType      = DRM_CSP_V2_HEADER;
    DRM_CHAR                *pstrRandomKeyID  = NULL;
    DRM_BYTE                *pbScratchKeySeed = NULL;

    ChkArg( argc == 6 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );
    ChkArg( argv[4] != NULL );
    ChkArg( argv[5] != NULL );

    dasstr.m_cch = (DRM_DWORD)DRMCRT_strlen( argv[0] ) + 1;
    dstrKeySeed.cchString = dasstr.m_cch;
    ChkMem( dstrKeySeed.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrKeySeed.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrKeySeed.pwszString, dstrKeySeed.cchString * SIZEOF( DRM_WCHAR ) );

    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( dstrKeySeed.cchString * SIZEOF( DRM_WCHAR ) ) );

    /*
    ** Safe because input parameter has ASCII symbols only
    */
    DRM_UTL_PromoteASCIItoUNICODE( argv[0], &dasstr, (DRM_STRING *)&dstrKeySeed );
    dstrKeySeed.cchString -= 1;

    dasstr.m_cch = (DRM_DWORD)DRMCRT_strlen( argv[1] ) + 1;
    dstrSigningKey.cchString = dasstr.m_cch;
    ChkMem(dstrSigningKey.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrSigningKey.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrSigningKey.pwszString, dstrSigningKey.cchString * SIZEOF( DRM_WCHAR ) );
    /*
    ** Safe because input parameter has ASCII symbols only
    */
    DRM_UTL_PromoteASCIItoUNICODE( argv[1], &dasstr, (DRM_STRING *)&dstrSigningKey );
    dstrSigningKey.cchString -= 1;

    dasstr.m_cch = (DRM_DWORD)DRMCRT_strlen( argv[2] ) + 1;
    dstrVerifyKey.cchString = dasstr.m_cch;
    ChkMem( dstrVerifyKey.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrVerifyKey.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrVerifyKey.pwszString, dstrVerifyKey.cchString * SIZEOF( DRM_WCHAR ) );
    /*
    ** Safe because input parameter has ASCII symbols only
    */
    DRM_UTL_PromoteASCIItoUNICODE( argv[2], &dasstr, (DRM_STRING *)&dstrVerifyKey );
    dstrVerifyKey.cchString -= 1;

    dasstr.m_cch = (DRM_DWORD)DRMCRT_strlen( argv[3] ) + 1;
    dstrLAUrl.cchString = dasstr.m_cch;
    ChkMem( dstrLAUrl.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrLAUrl.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrLAUrl.pwszString, dstrLAUrl.cchString * SIZEOF( DRM_WCHAR ) );
    /*
    ** Safe because input parameter has ASCII symbols only
    */
    DRM_UTL_PromoteASCIItoUNICODE( argv[3], &dasstr, (DRM_STRING *)&dstrLAUrl );
    dstrLAUrl.cchString -= 1;

    ChkDR( TST_UTL_MakePathString( NULL, argv[4], &dstrHeaderFile ) );

    dwHeaderVersion = atoi( argv[5] );
    switch( dwHeaderVersion )
    {
        case LIC_RESP_TEST_V2_HEADER:
            eHeaderType = DRM_CSP_V2_HEADER;
            cbContentKeySize = DRMCIPHERKEYLEN_RC4;
        break;
        case LIC_RESP_TEST_V4_HEADER:
            eHeaderType = DRM_CSP_V4_HEADER;
            cbContentKeySize = DRM_AES_KEYSIZE_128;
        break;
        case LIC_RESP_TEST_V4_1_HEADER:
            eHeaderType = DRM_CSP_V4_1_HEADER;
            cbContentKeySize = DRM_AES_KEYSIZE_128;
        break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }

    ChkMem( pCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    dr = DRM_HB_CreateHeader( NULL, eHeaderType, &cbContext );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    ChkMem( pbContext=(DRM_BYTE*)Oem_MemAlloc( cbContext ) );
    ZEROMEM( pbContext, cbContext );
    ChkDR(DRM_HB_CreateHeader( pbContext, eHeaderType, &cbContext ) );

    dr = DRM_KG_GenerateKeyID( NULL, NULL, &( dstrKeyID.cchString ) );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    ChkBOOL( dstrKeyID.cchString!=0, DRM_E_FAIL );
    ChkMem( dstrKeyID.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrKeyID.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrKeyID.pwszString, dstrKeyID.cchString * SIZEOF( DRM_WCHAR ) );
    ChkDR( DRM_KG_GenerateKeyID( NULL,(DRM_WCHAR *)dstrKeyID.pwszString, &(dstrKeyID.cchString) ) );

    /*
    ** Save the RANDOM_KID_VAR
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pstrRandomKeyID, RANDOM_KID_VAR, (dstrKeyID.cchString + 1) * SIZEOF( DRM_CHAR ) );
    ZEROMEM( pstrRandomKeyID, (dstrKeyID.cchString + 1) * SIZEOF( DRM_CHAR ) );

    /*
    ** Safe to use, input parameter is ASCII
    */
    DRM_UTL_DemoteUNICODEtoASCII( dstrKeyID.pwszString, pstrRandomKeyID, dstrKeyID.cchString );

    if( eHeaderType == DRM_CSP_V2_HEADER )
    {
        CallHeaderBuilder( pbContext, DRM_HB_SetLicAcqUrl( pbContext, &dstrLAUrl ) );
    }

    CallHeaderBuilder( pbContext, DRM_HB_SetKeyID(pbContext, (DRM_CONST_STRING*)&dstrKeyID ) );

    dr = DRM_KG_GenerateContentKeyB64(  dstrKeySeed.pwszString,
                                        dstrKeySeed.cchString,
                                        dstrKeyID.pwszString,
                                        dstrKeyID.cchString,
                                        DRM_CSP_V2_HEADER == eHeaderType ? eDRM_RC4_CIPHER : eDRM_AES_COUNTER_CIPHER,
                                        cbContentKeySize,
                                        NULL,
                                       &dstrContentKey.cchString,
                                        pbScratchKeySeed,
                                        dstrKeySeed.cchString * SIZEOF( DRM_WCHAR ) );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    ChkBOOL( dstrContentKey.cchString!=0, DRM_E_FAIL );
    ChkMem( dstrContentKey.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrContentKey.pwszString, dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) );

    ChkDR( DRM_KG_GenerateContentKeyB64(  dstrKeySeed.pwszString,
                                          dstrKeySeed.cchString,
                                          dstrKeyID.pwszString,
                                          dstrKeyID.cchString,
                                          DRM_CSP_V2_HEADER == eHeaderType ? eDRM_RC4_CIPHER : eDRM_AES_COUNTER_CIPHER,
                                          cbContentKeySize,
                                          dstrContentKey.pwszString,
                                         &dstrContentKey.cchString,
                                          pbScratchKeySeed,
                                          dstrKeySeed.cchString * SIZEOF( DRM_WCHAR ) ) );

    CallHeaderBuilder( pbContext, DRM_HB_SetChecksum( pbContext, (DRM_CONST_STRING*)&dstrContentKey) );
    CallHeaderBuilder( pbContext, DRM_HB_CloseHeader( pbContext, pCrypto, &dstrSigningKey,&pbHeaderXML, &cbHeaderXML ) );

    fp = Oem_File_Open( NULL, dstrHeaderFile.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );
    ChkBOOL( Oem_File_Write( fp, pbHeaderXML, cbHeaderXML, &cbWritten ), DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == cbHeaderXML, DRM_E_FILEWRITEERROR );


ErrorExit:
    SAFE_OEM_FREE( pCrypto );
    FREEDRMSTRING( dstrKeySeed );
    FREEDRMSTRING( dstrVerifyKey );
    FREEDRMSTRING( dstrSigningKey );
    FREEDRMSTRING( dstrContentKey );
    FREEDRMSTRING( dstrKeyID );
    FREEDRMSTRING( dstrHeaderFile );
    SAFE_OEM_FREE( pbContext );
    SAFE_OEM_FREE( pbScratchKeySeed );

    if( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }

    return dr;
}


EXIT_PKTEST_NAMESPACE_CODE

