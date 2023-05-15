/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>
#include <sys/stat.h>

#include <tOEMIMP.h>
#include <tclib.h>
#include <drmcontextsizes.h>
#include <tstutils.h>
#include <drmcrt.h>
#include <toem_paths.h>
#include <oemkeyfile.h>
#include <drmkeyfile.h>
#include <drmerror.h>
#include <drmwmdrm.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define MAX_COPY_BUFFER 12000

/*********************************************************
**
** Function: TST_OEM_IsConditionTrue
**
** Synopsis: Platform specific condition checking. Called by
**           TST_UTL_IsConditionTrue.
**
** Arguments:
**            f_pfCondition  -- TRUE if the codition matches
**            f_pszCondition -- String repesenting the condition
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_IsConditionTrue( __out DRM_BOOL       *f_pfCondition,
                                             __in  const DRM_CHAR *f_pszCondition )
{
    UNREFERENCED_PARAMETER( f_pfCondition );
    UNREFERENCED_PARAMETER( f_pszCondition );
    return DRM_SUCCESS;
}


/*
struct timespec {
    time_t tv_sec;        seconds
    long   tv_nsec;       nanoseconds
};
*/
#define NANOSECONDS_PER_MILLISECOND      1000000
#define MILLISECONDS_PER_SECOND             1000

DRM_VOID DRM_CALL tDRMSleep( DRM_DWORD dwMillSecond )
{
    DRM_RESULT      dr      = DRM_SUCCESS;
    DRM_DWORD       dwNSecs = 0;
    DRM_DWORD       dwSecs  = 0;
    DRM_DWORD       dwMills = 0;
    struct timespec ts;

    /* determine the full seconds part of the total ms */
    dwSecs = ( dwMillSecond / MILLISECONDS_PER_SECOND );

    /* determine the remaining ms part of the total ms */
    dwMills = ( dwMillSecond - ( dwSecs * MILLISECONDS_PER_SECOND ) );

    /* covert the remaining ms into ns */
    ChkDR( DRM_DWordMult( dwMills, NANOSECONDS_PER_MILLISECOND, &dwNSecs ) );


    ts.tv_sec  = dwSecs;
    ts.tv_nsec = dwNSecs;
    if ( nanosleep( &ts, NULL ) != 0 )
    {
        Log("Trace"," \t\tFrom tDRMSleep - nanosleep() failed with errno %d\n", DRM_RESULT_FROM_WIN32( errno ) );
        DRMASSERT(FALSE);
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        Log("Trace"," \t\tFrom tDRMSleep - failed with dr %d\n", dr );
        DRMASSERT(FALSE);
    }
    return;
}


DRM_BOOL DRM_CALL tMoveFile(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszTarget)
{
    DRMASSERT(FALSE);
    return FALSE;
}

DRM_RESULT DRM_CALL tRemoveFile(const DRM_CHAR* filename)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( filename != NULL );

    if ( 0 != remove( filename ) )
    {
        /*
        ** It is not an error if the file does not exist.
        */
        if ( errno != ENOENT )
        {
            Log("Trace"," \t\tFrom tRemoveFile - Failed to Remove file %s\n", filename );
            ChkDR( DRM_RESULT_FROM_WIN32( errno ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL tRemoveFileW( const DRM_WCHAR *wcszPathname )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_CHAR  *szPathName  = NULL;
    DRM_DWORD  cchPathName = 0;


    cchPathName = ( DRM_DWORD ) DRMCRT_wcslen( wcszPathname );
    ChkMem( szPathName = ( char * )Oem_MemAlloc( cchPathName + 1 ) );
    ZEROMEM( szPathName, cchPathName + 1 );
    DRM_UTL_DemoteUNICODEtoASCII(wcszPathname, szPathName, cchPathName + 1 );

    ChkDR( tRemoveFile( szPathName ) );

ErrorExit:
    SAFE_OEM_FREE( szPathName );
    return dr;
}

DRM_RESULT DRM_CALL RemoveDRMFile(DRM_DWORD dwRemove)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrpath;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    if ( dwRemove & RMFILE_STORE )
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrpath) );
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PD_DEVCERT )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, DEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_DEVCERT )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, BDEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_ND_DEVCERT )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, NDR_CERT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PD_DEVCERTTEMPLATE )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, DEVCERT_TEMPLATE_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_DEVCERTTEMPLATE )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, GROUP_CERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));

    }

    if ( dwRemove & RMFILE_ND_DEVCERTTEMPLATE )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_CERT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PD_PRVKEY )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_GROUP_PRVKEY )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, GC_PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_PRVKEY )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_SIGN));
        ChkDR(tRemoveFileW(dstrpath.pwszString));

        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_ND_GROUP_PRVKEY )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_ND_PRVKEY )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, NDR_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_KEYFILE )
    {
        ChkDR(tGetDRMFileName(wszPathName, &dstrpath, KEYFILE_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

ErrorExit:
    return dr;
}

DRM_BOOL DRM_CALL _IsDebuggerAttached()
{
    return FALSE;
}

static DRM_KEYFILE_CONTEXT s_tempKeyfileContext = {0};
DRM_RESULT DRM_CALL _StoreCertInKeyfile( DRM_KF_CERT_TYPE f_eCertType, DRM_BYTE *pbCert, DRM_DWORD cbCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fKeyfileOpen = FALSE;
    DRM_BOOL fKeyfileInitialized = FALSE;
    PRIVKEY    GCPrivkey   = {0};
    DRM_DWORD  cbGCPrivKey = SIZEOF( PRIVKEY );

    ChkArg( f_eCertType != eKF_CERT_TYPE_INVALID );
    ChkArg( pbCert != NULL );
    ChkArg( cbCert != 0 );

    ChkDR( DRM_KF_Initialize( NULL, &s_tempKeyfileContext ) );
    fKeyfileInitialized = TRUE;
    ChkDR( DRM_KF_Open( &s_tempKeyfileContext, TRUE,
                       OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    fKeyfileOpen = TRUE;
    ChkDR( DRM_KF_LoadFromFile( &s_tempKeyfileContext, TRUE ) );

    ChkDR( DRM_KF_SetCertificate( &s_tempKeyfileContext, f_eCertType, pbCert, cbCert ) );

    if( f_eCertType == eKF_CERT_TYPE_WMDRM )
    {
        DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;
        PUBKEY pubkeyCurrent = {0};
        PRIVKEY privkeyCurrent = {0};
        DRM_DWORD cb = 0;
        DRM_ENCRYPTED_KEY privkey = {0};
        DRM_BYTE rgbKeyBuffer[128] = {0};

        dstrDevCert.pwszString = (DRM_WCHAR*)pbCert;
        dstrDevCert.cchString = cbCert / SIZEOF( DRM_WCHAR );

        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPUBKEY, NULL, &dstrValue) );
        cb = SIZEOF( PUBKEY );
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                       &cb,
                                       (DRM_BYTE*)&pubkeyCurrent,
                                       0)))
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }

        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPRIVKEY, NULL, &dstrValue) );

        if ( dstrValue.cchString != CCH_BASE64_EQUIV( SIZEOF(PRIVKEY) ) )
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
            goto ErrorExit;
        }

        cb = SIZEOF(PRIVKEY);
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                       &cb,
                                       (DRM_BYTE*)&privkeyCurrent,
                                       0)))
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }

        ChkDR( Oem_Device_GetKey( NULL,
                                 DRM_DKT_WMDRMPD_GROUP,
                                 (DRM_BYTE *)&GCPrivkey,
                                 &cbGCPrivKey ) );

        ChkDR( DRM_BBX_Legacy_SymmetricCipher( FALSE,
                                               ( DRM_BYTE * )&GCPrivkey,
                                               SIZEOF( GCPrivkey ),
                                               ( DRM_BYTE * )&privkeyCurrent,
                                               ( DRM_BYTE * )&privkeyCurrent,
                                               SIZEOF( PRIVKEY ) ) );

        privkey.cbEncryptedKey = SIZEOF(rgbKeyBuffer);
        privkey.pbEncryptedKey = rgbKeyBuffer;

        ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( NULL,
                                              (DRM_BYTE*)&privkeyCurrent,
                                              SIZEOF(PRIVKEY),
                                              (DRM_DEVICE_KEY_TYPE)0,
                                              &privkey ) );

        ChkDR( DRM_KF_SetPrivateKey( &s_tempKeyfileContext,
                                    eKF_KEY_TYPE_ECC_160,
                                    (DRM_BYTE*)&pubkeyCurrent,
                                    SIZEOF(PUBKEY),
                                    &privkey ) );
    }

    ChkDR( DRM_KF_StoreToFile( &s_tempKeyfileContext ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( (DRM_BYTE*)&GCPrivkey, SIZEOF( PRIVKEY ) );  /* trash the key in memory */
    if( fKeyfileOpen )
    {
        DRM_KF_Close( &s_tempKeyfileContext );
    }
    if( fKeyfileInitialized )
    {
        DRM_KF_Uninitialize( &s_tempKeyfileContext );
    }
    return dr;
}


static DRM_RESULT _SymmetricCipher(
    __in                                DRM_BOOL  f_fEncrypt,
    __in_bcount_opt( f_cbKey )   const  DRM_BYTE *f_pbKey,
    __in                                DRM_DWORD f_cbKey,
    __in_bcount( f_cbData )             DRM_BYTE *f_pbDataIn,
    __out_bcount_opt( f_cbData )        DRM_BYTE *f_pbDataOut,
    __in                                DRM_DWORD f_cbData )
{
    DRM_RESULT                  dr                                                            = DRM_SUCCESS;
    DRM_BYTE                   *rgbOutputKeyAligned                                           = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT stkContext                                                    = {0};
    DRM_AES_KEY                 aesKey                                                        = {0};
    DRM_BYTE                    rgbOutputKeyUnaligned[ DRM_AES_BLOCKLEN + SIZEOF(DRM_DWORD) ] = {0};
    DRM_BYTE                    rgbConstantBlock[DRM_AES_BLOCKLEN]                            = {0};

    ChkArg( f_pbDataIn  != NULL );
    ChkArg( f_pbKey     != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_cbKey > 0 );

    if ( f_pbDataOut != NULL )
    {
        MEMCPY( f_pbDataOut, f_pbDataIn, f_cbData );
    }
    else
    {
        f_pbDataOut = f_pbDataIn;
    }

    /*
    **  1. Get an OMAC1 of the privkey
    **  2. Generate an AES key using this hash
    **  3. Decrypt or encrypt ciphertext using this AES key
    */

    /*
    ** rgbOutputKeyAligned will be cast to a DWORD aligned
    ** structure in Oem_Aes_SetKey so make sure rgbOutputKeyAligned
    ** is DWORD aligned.
    */
    ChkDR( DRM_STK_Init( &stkContext,
                            rgbOutputKeyUnaligned,
                            SIZEOF( rgbOutputKeyUnaligned ) ) );

    ChkDR( DRM_STK_Alloc_Aligned( &stkContext,
                                    DRM_AES_BLOCKLEN,
                                    SIZEOF( DRM_DWORD ),
                                    NULL,
                                    ( DRM_VOID **) &rgbOutputKeyAligned ) );

    ChkBOOL( f_cbKey >= DRM_AES_KEYSIZE_128, DRM_E_CH_BAD_KEY );
    ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
    ChkDR( Oem_Omac1_Sign(&aesKey, rgbConstantBlock, 0, SIZEOF(rgbConstantBlock), rgbOutputKeyAligned ) );
    ChkDR( Oem_Aes_ZeroKey(&aesKey) );

    ChkDR( Oem_Aes_SetKey( rgbOutputKeyAligned, &aesKey ) );

    if ( f_fEncrypt )
    {
        ChkDR( Oem_Aes_EcbEncryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }
    else
    {
        ChkDR( Oem_Aes_EcbDecryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( &aesKey );
    OEM_SECURE_ZERO_MEMORY( rgbOutputKeyUnaligned, SIZEOF( rgbOutputKeyUnaligned ) );
    return dr;
}


DRM_RESULT DRM_CALL _StoreKeyInKeyfile( DRM_KF_KEY_TYPE f_eKeyType, DRM_DEVICE_KEY_TYPE f_eDeviceKeyType, DRM_BYTE *pbPubKey, DRM_DWORD cbPubKey, DRM_BYTE *pbKey, DRM_DWORD cbKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fKeyfileOpen = FALSE;
    DRM_BOOL fKeyfileInitialized = FALSE;
    DRM_ENCRYPTED_KEY privkey = {0};
    PRIVKEY_P256    GCPrivKey   = {0};
    DRM_DWORD       cbGCPrivKey = SIZEOF( PRIVKEY_P256 );
    DRM_BYTE rgbKeyBuffer[128] = {0};

    ChkArg( pbPubKey != NULL );
    ChkArg( pbKey != NULL );

    ChkDR( DRM_KF_Initialize( NULL, &s_tempKeyfileContext ) );
    fKeyfileInitialized = TRUE;
    ChkDR( DRM_KF_Open( &s_tempKeyfileContext, TRUE,
                       OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    fKeyfileOpen = TRUE;
    ChkDR( DRM_KF_LoadFromFile( &s_tempKeyfileContext, TRUE ) );

    if( f_eKeyType == eKF_KEY_TYPE_ECC_160 )
    {
        ChkArg( cbPubKey == SIZEOF( PUBKEY ) );
        ChkArg( cbKey == SIZEOF( PRIVKEY ) );
    }
    else if( f_eKeyType == eKF_KEY_TYPE_ECC_256 )
    {
        ChkArg( cbPubKey == SIZEOF( PUBKEY_P256) );
        ChkArg( cbKey == SIZEOF( PRIVKEY_P256 ) );

        ChkDR( Oem_Device_GetKey( NULL,
                                 DRM_DKT_PLAYREADY_MODEL,
                                 (DRM_BYTE *)&GCPrivKey,
                                 &cbGCPrivKey ) );

        ChkDR( _SymmetricCipher( FALSE,
                                (DRM_BYTE *)&GCPrivKey,
                                SIZEOF( PRIVKEY_P256 ),
                                pbKey,
                                ( DRM_BYTE * )pbKey,
                                SIZEOF( PRIVKEY_P256 ) ) );
    }
    else
    {
        ChkArg( FALSE );
    }
    privkey.cbEncryptedKey = SIZEOF(rgbKeyBuffer);
    privkey.pbEncryptedKey = rgbKeyBuffer;

    ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( NULL,
                                          pbKey,
                                          cbKey,
                                          (DRM_DEVICE_KEY_TYPE)f_eDeviceKeyType,
                                          &privkey ) );

    ChkDR( DRM_KF_SetPrivateKey( &s_tempKeyfileContext, f_eKeyType, pbPubKey, cbPubKey, &privkey ) );
    ChkDR( DRM_KF_StoreToFile( &s_tempKeyfileContext ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( (DRM_BYTE*)&GCPrivKey, SIZEOF( PRIVKEY_P256 ) );  /* trash the key in memory */

    if( fKeyfileOpen )
    {
        DRM_KF_Close( &s_tempKeyfileContext );
    }
    if( fKeyfileInitialized )
    {
        DRM_KF_Uninitialize( &s_tempKeyfileContext );
    }
    return dr;
}

DRM_RESULT DRM_CALL _CopyFileToDRMFolder(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszDest)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrDest = {0};
    DRM_DWORD cbData;
    DRM_BYTE *pbData = NULL;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    if (!wszSrc)
    {
        goto ErrorExit;
    }

    if( !DRM_WMDRM_IsWMDRMSupported() )
    {
        /*
        ** Skip WMDRM-specific files
        */
        if( ( DRMCRT_wcsicmp( wszDest, DEVCERT_FILE_NAME )          == 0 )
         || ( DRMCRT_wcsicmp( wszDest, DEVCERT_TEMPLATE_FILE_NAME ) == 0 )
         || ( DRMCRT_wcsicmp( wszDest, PRIVATE_KEY_FILE_NAME )      == 0 )
         || ( DRMCRT_wcsicmp( wszDest, WMDRMPD_KEY_FILE_NAME )      == 0 ) )
        {
            goto ErrorExit;
        }
    }

    ChkBOOL(LoadTestFileW(NULL, wszSrc, &pbData, &cbData), DRM_E_FILE_READ_ERROR);

    if( DRMCRT_wcsicmp( wszDest, DEVCERT_FILE_NAME ) == 0 )
    {
        ChkDR( _StoreCertInKeyfile( eKF_CERT_TYPE_WMDRM, pbData, cbData ) );
    }
    else if( DRMCRT_wcsicmp( wszDest, BDEVCERT_FILE_NAME ) == 0 )
    {
        ChkDR( _StoreCertInKeyfile( eKF_CERT_TYPE_PLAYREADY, pbData, cbData ) );
    }
    else if( DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT ) == 0
            || DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_SIGN ) == 0)
    {
        if( cbData == SIZEOF( PRIVKEY_P256 ) + SIZEOF( PUBKEY_P256 ) )
        {
            DRM_DEVICE_KEY_TYPE eDeviceKeyType = DRM_DKT_INVALID;

            if( DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT ) == 0 )
            {
                eDeviceKeyType = DRM_DKT_PLAYREADY_DEVICE_ENCRYPT;
            }
            else
            {
                eDeviceKeyType = DRM_DKT_PLAYREADY_DEVICE_SIGN;
            }

            ChkDR( _StoreKeyInKeyfile( eKF_KEY_TYPE_ECC_256,
                                      eDeviceKeyType,
                                      pbData + SIZEOF( PRIVKEY_P256 ),
                                      SIZEOF( PUBKEY_P256 ),
                                      pbData,
                                      SIZEOF(PRIVKEY_P256 ) ) );
        }
    }

    ChkDR(tGetDRMFileName(wszPathName, &dstrDest, wszDest));
    ChkBOOL(SaveToFileW(dstrDest.pwszString, pbData, cbData), DRM_E_FILE_WRITE_ERROR);

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}

static DRM_RESULT _tGetTCFileListImp( __in_z char *szFileNameTemplate, __in_z char **szPlatforms, __inout_ecount_z_opt( *pdwCount ) char **szFileNames, __inout DRM_DWORD *pdwCount)
{

    DRM_RESULT      dr          = DRM_SUCCESS;
    glob_t          gFiles      = { 0 };
    DRM_DWORD       dwFound     = 0;
    DRM_DWORD       cchFilename = 0;
    DRM_LONG        nGlobErr    = 0;
    DRM_ANSI_STRING dastrFileNameTemplate = { NULL, 0 };
    DRM_LONG nFile = 0;
    DRM_CHAR **szPlatform = NULL;

    ChkArg( NULL != szFileNameTemplate );
    ChkArg( NULL != szPlatforms );
    ChkArg( NULL != pdwCount );

    ChkDR( TST_UTL_MakePathA(NULL, szFileNameTemplate, &dastrFileNameTemplate.pszString, &dastrFileNameTemplate.cchString, FALSE) );

    nGlobErr = glob(dastrFileNameTemplate.pszString, 0, NULL, &gFiles);
    if ( GLOB_NOSPACE == nGlobErr && errno != 0)
    {
        ChkDR( DRM_E_OUTOFMEMORY );
    }
    else if ( GLOB_NOMATCH == nGlobErr )
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }
    else if ( nGlobErr != 0 )
    {
        ChkDR( DRM_E_FAIL );
    }

    for ( nFile = 0; gFiles.gl_pathv[nFile] != NULL ; nFile++ )
    {
        for ( szPlatform = szPlatforms; *szPlatform != NULL; szPlatform++ )
        {
            if (strstr( gFiles.gl_pathv[nFile], *szPlatform ) ) {

                if ( NULL != szFileNames && dwFound < *pdwCount )
                {
                    cchFilename = strlen(gFiles.gl_pathv[nFile]);
                    ChkMem( szFileNames[dwFound] = (char *) Oem_MemAlloc(cchFilename + 1) );
                    MEMCPY( szFileNames[dwFound], gFiles.gl_pathv[nFile], cchFilename );
                    szFileNames[dwFound][cchFilename] = '\0';
                }
                dwFound++;
                break;
            }
        }
    }

    if ( NULL == szFileNames || dwFound > *pdwCount )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *pdwCount = dwFound;
ErrorExit:
    globfree( &gFiles );
    SAFE_OEM_FREE( dastrFileNameTemplate.pszString );
    return dr;

}

DRM_RESULT DRM_CALL tGetTCFileList(const char *szTestFilePrefix, const char **szPlatforms, const char ***pszTestFileNames)
{
    DRM_RESULT dr           = DRM_SUCCESS;
    char       szFN[260]    = {0};
    char      *pszWildCards = ".*.txt";
    DRM_DWORD  cchWildcards = ( DRM_DWORD ) strlen( pszWildCards );
    DRM_DWORD  dwCount      = 0;
    DRM_DWORD  cchPrefix    = 0;

    ChkArg( szTestFilePrefix );
    ChkArg( szPlatforms );
    ChkArg( pszTestFileNames );

    cchPrefix = ( DRM_DWORD ) strlen( szTestFilePrefix );

    ChkOverflow( cchPrefix + cchWildcards, cchPrefix );

    ChkArg( SIZEOF( szFN ) > cchPrefix + cchWildcards );

    MEMCPY(szFN, szTestFilePrefix, cchPrefix );
    DRM_BYT_CopyBytes( szFN, cchPrefix, pszWildCards, 0, cchWildcards );
    szFN[ cchPrefix + cchWildcards ] = '\0';

    if (DRM_E_BUFFERTOOSMALL != (dr = _tGetTCFileListImp(szFN, (char **)szPlatforms, NULL, &dwCount)))
    {
        ChkDR(dr);
    }

    ChkMem(*pszTestFileNames = (const char**)Oem_MemAlloc((dwCount + 1) * SIZEOF(char*)));
    ZEROMEM((char **)*pszTestFileNames, (dwCount + 1) * SIZEOF(char*));
    ChkDR(_tGetTCFileListImp(szFN, (char **)szPlatforms, (char **)*pszTestFileNames, &dwCount));
ErrorExit:
    return dr;
}

void DRM_CALL tFreeFileNameList(__in_opt const char **szTestFileNames)
{
    if (szTestFileNames)
    {
        const char **p = NULL;
        for ( p = szTestFileNames; *p; p++ )
        {
            SAFE_OEM_FREE(*p);
        }
        SAFE_OEM_FREE(szTestFileNames);
    }
}


/* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms.  We want 1 ms resolution. */
DRM_UINT64 DRM_CALL GetTimer (void)
{
    return DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                     Oem_Clock_QueryPerformanceCounter( NULL ) ),
                        Oem_Clock_QueryPerformanceFrequency( NULL ) );
}


DRM_VOID DRM_CALL tOEMSignalTestDone()
{
}

/*********************************************************
**
** Function: TST_OEM_ExecuteShellCommand
**
** Synopsis: Executes a commandline and writes the output to the log
**
** Arguments:
**            f_szCommandLine -- Commond Line to execute
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_ExecuteShellCommand( __in_z DRM_CHAR *f_szCommandLine )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_E_TEST_NOTIMPL );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandStart(
    __in_z                        const DRM_CHAR                         *f_szCommandLine,
    __out                               DRM_VOID                        **f_ppvAsyncShellCommandData )
{
    return DRM_E_TEST_NOTIMPL;
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandEnd(
    __inout                             DRM_VOID                        **f_ppvAsyncShellCommandData )
{
    return DRM_E_TEST_NOTIMPL;
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandPing(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMs )
{
    return DRM_E_TEST_NOTIMPL;
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandReadMessage(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __out                               DRM_DWORD                        *f_pdwMessageType,
    __deref_out_ecount(*f_pcbMessage)   DRM_BYTE                        **f_ppbMessage,
    __out                               DRM_DWORD                        *f_pcbMessage )
{
    return DRM_E_TEST_NOTIMPL;
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandWriteMessage(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMessageType,
    __in_ecount(f_cbMessage)      const DRM_BYTE                         *f_pbMessage,
    __in                                DRM_DWORD                         f_cbMessage )
{
    return DRM_E_TEST_NOTIMPL;
}

/*********************************************************
**
** Function: TST_OEM_CopyFile
**
** Synopsis: creates a directory at the specified path
**
** Arguments:
**  f_dstrSource -- File to copy
**  f_dstrDest   -- Destination for the copy
**
** Returns     DRM_SUCCESS if the directory is created
**             DRM_S_FALSE if the directory already exists
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_CopyFile( __in DRM_CONST_STRING f_dstrSource,
                                      __in DRM_CONST_STRING f_dstrDest )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    OEM_FILEHDL fp_src       = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL fp_dst       = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   dwFileSize   = 0;
    DRM_DWORD   dwData       = 0;
    DRM_DWORD   dwWritten    = 0;
    DRM_DWORD   dwCopyBuffer = 0;
    DRM_DWORD   dwCopied     = 0;
    DRM_BYTE   *pbData       = NULL;

    ChkArg( f_dstrSource.pwszString != NULL );
    ChkArg( f_dstrDest.pwszString   != NULL );

    fp_src = Oem_File_Open( NULL, f_dstrSource.pwszString, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );

    if( fp_src == OEM_INVALID_HANDLE_VALUE )
    {
        TRACE( ( "TST_OEM_CopyFile failed to open source file %S.", f_dstrSource.pwszString ) );
        ChkDR( DRM_E_FILEOPEN );
    }

    if( !Oem_File_GetSize( fp_src, &dwFileSize ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    fp_dst = Oem_File_Open( NULL, f_dstrDest.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );

    if( fp_dst == OEM_INVALID_HANDLE_VALUE )
    {
        /* Try deleting the file, it may be hidden or read-only */
        ChkDR( tRemoveFileW( f_dstrDest.pwszString ) );

        /* Try to open it again */
        fp_dst = Oem_File_Open( NULL, f_dstrDest.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );

        if( fp_dst == OEM_INVALID_HANDLE_VALUE )
        {
            TRACE( ( "TST_OEM_CopyFile failed to open destination file %S.", f_dstrDest.pwszString ) );
            ChkDR( DRM_E_FILEOPEN );
        }
    }

    if( dwFileSize > MAX_COPY_BUFFER )
    {
        dwCopyBuffer = MAX_COPY_BUFFER;
    }
    else
    {
        dwCopyBuffer = dwFileSize;
    }

    ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( dwCopyBuffer + SIZEOF( DRM_WCHAR ) ) );

    while( dwCopied < dwFileSize )
    {
        ChkBOOL( Oem_File_Read( fp_src, pbData, dwCopyBuffer, &dwData ), DRM_E_FILE_READ_ERROR );
        ChkBOOL( Oem_File_Write( fp_dst, pbData, dwData, &dwWritten ), DRM_E_FILE_WRITE_ERROR );
        ChkBOOL( dwData == dwWritten, DRM_E_FILE_WRITE_ERROR );

        dwCopied += dwData;
        ZEROMEM( pbData, SIZEOF( pbData ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbData );

    if ( fp_src != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp_src );
    }
    if ( fp_dst != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp_dst );
    }

    return dr;
}

/*********************************************************
**
** Function: TST_OEM_CreateDirectory
**
** Synopsis: creates a directory at the specified path
**
** Arguments:
**  f_szPath -- Directory to create.
**
** Returns     DRM_SUCCESS if the directory is created
**             DRM_S_FALSE if the directory already exists
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_CreateDirectory( __in_z const DRM_CHAR *f_szPath )
{
    DRM_RESULT dr  = DRM_SUCCESS;

    ChkArg( NULL != f_szPath );

    if(0 != mkdir( (DRM_CHAR*)f_szPath, 0777 ) )
    {
        if ( EEXIST == errno )
        {
            dr = DRM_S_FALSE;
        }
        else
        {
            perror( (DRM_CHAR*)f_szPath );
            ChkDR( DRM_E_FAIL );
        }
    }

ErrorExit:
    return dr;
}

DRM_VOID DRM_CALL TST_OEM_LeakScan() {}
DRM_VOID DRM_CALL TST_OEM_LeakClear() {}

EXIT_PKTEST_NAMESPACE_CODE;
