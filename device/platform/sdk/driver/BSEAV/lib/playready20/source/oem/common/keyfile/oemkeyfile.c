/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <oemkeyfile.h>
#include <oemaes.h>

ENTER_PK_NAMESPACE_CODE;

/*
** This is a TEST key meant for testing purposes ONLY.
** OEMs must generate a new AES key and protect it as a
** Device Asset per PlayReady Compliance and Robustness Rules.
** This structure is declared in oemkeyfile.h, but
** an OEM must also create a new implementation of
** Oem_Keyfile_GetKFAesKey and Oem_Keyfile_GetCurrentKFAesKeyId
** to access the newly generated key in a robust way,
** so it is unlikely that an OEM will continue to use this
** structure in their final implementation.
*/
const DRM_EXPORT_VAR DRM_KEYHISTORY_ENTRY g_oTestKeyfileKey =
    {
        {
            0x0001D3BE,
            0xD26A,
            0x44F1,
            {
                0x9E, 0x42, 0xAD, 0xF6,
                0x63, 0xBF, 0x2B, 0x2C
            }
        },
        {
            0x8B, 0x22, 0x2F, 0xFD, 0x1E, 0x76, 0x19, 0x56,
            0x59, 0xCF, 0x27, 0x03, 0x89, 0x8C, 0x42, 0x7F
        }
    };

static const DRM_KEYHISTORY_ENTRY* g_rgoKeyHistory[] = { &g_oTestKeyfileKey };
DRM_EXPORT_VAR DRM_KEYHISTORY_ENTRY **g_ppKeyHistoryList   = (DRM_KEYHISTORY_ENTRY **)&g_rgoKeyHistory;

DRM_EXPORT_VAR DRM_DWORD             g_dwKeyHistoryCount = NO_OF(g_rgoKeyHistory);

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
/*
** Robustness version and platform ID for DRM_BUILD_PROFILE_WM7
*/
DRM_EXPORT_VAR DRM_DWORD g_dwWMRobustnessVersion = 0x00000001;
DRM_EXPORT_VAR DRM_DWORD g_dwPRRobustnessVersion = 0x00000001;

DRM_EXPORT_VAR DRM_DWORD g_dwPlatformID = 0x00000005;
#elif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS
/*
** Robustness version and platform ID for DRM_BUILD_PROFILE_IOS
*/
DRM_EXPORT_VAR DRM_DWORD g_dwWMRobustnessVersion = 0x00000103;
DRM_EXPORT_VAR DRM_DWORD g_dwPRRobustnessVersion = 0x00000103;

DRM_EXPORT_VAR DRM_DWORD g_dwPlatformID = 0x00000005;
#elif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_XBOX
/*
** Robustness version and platform ID for DRM_BUILD_PROFILE_XBOX
*/
DRM_EXPORT_VAR DRM_DWORD g_dwWMRobustnessVersion = 0x1000001;
DRM_EXPORT_VAR DRM_DWORD g_dwPRRobustnessVersion = 0x1000001;

DRM_EXPORT_VAR DRM_DWORD g_dwPlatformID = 0x00000007;

#else
/*
** Robustness version and platform ID for all other profiles
*/
DRM_EXPORT_VAR DRM_DWORD g_dwWMRobustnessVersion = 0x00000000;
DRM_EXPORT_VAR DRM_DWORD g_dwPRRobustnessVersion = 0x00000000;

DRM_EXPORT_VAR DRM_DWORD g_dwPlatformID = 0x00000000;
#endif

/*
** This variable is used to store either the file name of the key file if
** default key file is going to be used, or the full path of the key file
** otherwise.
*/
DRM_EXPORT_VAR DRM_WCHAR g_rgwchKeyFileName[ KF_MAX_FILE_PATH ] =
{
    ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('f', '\0'),
    ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
};

/*
** Flag indicating whether the default key file should be used.
** If it is FALSE, user should supply a full path of the key file in g_rgwchKeyFileName.
** If it is TRUE, user should provide the file name of the key file in g_rgwchKeyFileName.
*/
DRM_EXPORT_VAR DRM_BOOL g_fUseDefaultKeyFile = TRUE;


DRM_API DRM_RESULT Oem_Keyfile_GetKFAesKey(
    __in                const DRM_GUID *f_pguidAesKeyID,
    __inout_ecount( 1 ) DRM_AES_KEY    *f_pAesKey )
{
    DRM_RESULT   dr      = DRM_SUCCESS;
    DRM_DWORD    dwCount = 0;

    ChkArg( f_pguidAesKeyID != NULL );
    ChkArg( f_pAesKey != NULL );

    DRMASSERT( g_dwKeyHistoryCount > 0 );

    dr = DRM_E_KEYFILE_KEY_NOT_FOUND;
    while( dwCount < g_dwKeyHistoryCount )
    {
        if( IDENTICAL_GUIDS( f_pguidAesKeyID, &(g_ppKeyHistoryList[dwCount]->guidKeyFileAESKeyID) ) )
        {
            /*
            ** Found the key
            */
            ChkDR( Oem_Aes_SetKey( g_ppKeyHistoryList[dwCount]->rgbKeyFileAESKey, f_pAesKey ) );
            break;
        }
        dwCount++;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT Oem_Keyfile_GetCurrentKFAesKeyId(
    __out DRM_GUID *f_pguidAesKeyID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pguidAesKeyID != NULL );

    DRMASSERT( g_dwKeyHistoryCount > 0 );

    /*
    ** Current key ID is the LAST in the history list.
    */
    MEMCPY( f_pguidAesKeyID, &(g_ppKeyHistoryList[g_dwKeyHistoryCount - 1]->guidKeyFileAESKeyID), SIZEOF(DRM_GUID) );

ErrorExit:
    return DRM_SUCCESS;
}

/******************************************************************************************
** Function:  Oem_Keyfile_Open
**
** Synopsis:  Opens the keyfile and returns a filehandle that may be used
**            with Oem_File_Read/Write/SetFilePointer etc.
**
** Arguments: [f_pOEMContext]           : A pointer to the OEM context; may be NULL
**            [f_dwAccessMode]          : File access mode. See oemcommon.h
**                                        for allowed values of the input parameters.
**            [f_dwShareMode]           : File opening share mode
**            [f_dwCreationDisposition] : File creation disposition
**            [f_dwAttributes]          : File attributes
**
** Returns:   Valid file handle on success, otherwise OEM_INVALID_HANDLE_VALUE.
**
**
** Notes:     The caller has to call Oem_File_... functions to perform the rest
**            of file operations.
******************************************************************************************/
DRM_API OEM_FILEHDL Oem_Keyfile_Open(
    __in_opt DRM_VOID   *f_pOEMContext,
    __in     DRM_DWORD   f_dwAccessMode,
    __in     DRM_DWORD   f_dwShareMode,
    __in     DRM_DWORD   f_dwCreationDisposition,
    __in     DRM_DWORD   f_dwAttributes )
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_WCHAR   *pwszPath    = NULL;
    OEM_FILEHDL  fFileHdl    = OEM_INVALID_HANDLE_VALUE;

    /*
    ** If g_fUseDefaultKeyFile is TRUE, let the system figure out the full path of
    ** the key file; otherwise just use the full path in g_rgwchKeyFileName that is
    ** provided by the user.
    */
    if( g_fUseDefaultKeyFile )
    {
        DRM_CONST_STRING dstrKeyFileName = EMPTY_DRM_STRING;

        dstrKeyFileName.pwszString = g_rgwchKeyFileName;
        dstrKeyFileName.cchString = DRMCRT_wcslen( g_rgwchKeyFileName );

        ChkBOOL( dstrKeyFileName.cchString < KF_MAX_FILE_PATH, DRM_E_INVALIDARG );

        ChkDR( _GetDRMFullPathName( &pwszPath, &dstrKeyFileName, KEYFILEPATH) );
    }
    else
    {
        pwszPath = g_rgwchKeyFileName;
    }

    fFileHdl = Oem_File_Open( f_pOEMContext,
                              pwszPath,
                              f_dwAccessMode,
                              f_dwShareMode,
                              f_dwCreationDisposition,
                              f_dwAttributes );
ErrorExit:

    if( g_fUseDefaultKeyFile )
    {
        SAFE_OEM_FREE( pwszPath );
    }

    return fFileHdl;

}

DRM_API DRM_RESULT Oem_Keyfile_Delete( )
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_WCHAR   *pwszPath    = NULL;

    /*
    ** If g_fUseDefaultKeyFile is TRUE, let the system figure out the full path of
    ** the key file; otherwise just use the full path in g_rgwchKeyFileName that is
    ** provided by the user.
    */
    if( g_fUseDefaultKeyFile )
    {
        DRM_CONST_STRING dstrKeyFileName = EMPTY_DRM_STRING;

        dstrKeyFileName.pwszString = g_rgwchKeyFileName;
        dstrKeyFileName.cchString = DRMCRT_wcslen( g_rgwchKeyFileName );

        ChkBOOL( dstrKeyFileName.cchString < KF_MAX_FILE_PATH, DRM_E_INVALIDARG );

        ChkDR( _GetDRMFullPathName( &pwszPath, &dstrKeyFileName, KEYFILEPATH) );
    }
    else
    {
        pwszPath = g_rgwchKeyFileName;
    }

    {
        OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;
        if( OEM_INVALID_HANDLE_VALUE != ( hFile = Oem_File_Open(
            NULL,
            pwszPath,
            OEM_GENERIC_READ,
            OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE,
            OEM_OPEN_EXISTING,
            0 ) ) )
        {
            (void)Oem_File_Close( hFile );
            if( !Oem_File_Delete( pwszPath ) )
            {
                ChkDR( DRM_E_FILEWRITEERROR );
            }
        }
    }
ErrorExit:

    if( g_fUseDefaultKeyFile )
    {
        SAFE_OEM_FREE( pwszPath );
    }

    return dr;
}

/******************************************************************************************
** Function:  Oem_Keyfile_Close
**
** Synopsis:  Closes the keyfile
**
** Arguments: [f_fHandle]       : File handle
**
** Returns:   None
**
******************************************************************************************/
DRM_VOID DRM_API Oem_Keyfile_Close(
    __in OEM_FILEHDL f_fHandle )
{
    Oem_File_Close( f_fHandle );
}




/*****************************************************************************************************************
** Function:    Oem_Keyfile_GetRobustnessVersion
**
** Synopsis:    Retrieves the current robustness version and platform ID.
**
** Arguments:   [f_pOEMContext]          : A pointer to the OEM context; may be NULL
**              [f_eCertType]            : A certificate type, tells which robustness version to return
**              [f_pdwRobustnessVersion] : Returns the current robustness version of the platform
**              [f_pdwPlatform]          : Returns the platform ID as used in the binary cert robustness version object
**
*****************************************************************************************************************/
DRM_RESULT DRM_API Oem_Keyfile_GetRobustnessVersion(
    DRM_VOID    *f_pOEMContext,
    DRM_KEYFILE_CERT_TYPE f_eCertType,
    DRM_DWORD   *f_pdwRobustnessVersion,
    DRM_DWORD   *f_pdwPlatform )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdwRobustnessVersion != NULL );
    ChkArg( f_pdwPlatform != NULL );

    switch( f_eCertType )
    {
    case KF_CERT_TYPE_WMDRM:
        *f_pdwRobustnessVersion = g_dwWMRobustnessVersion;
        break;
    case KF_CERT_TYPE_PLAYREADY:
        *f_pdwRobustnessVersion = g_dwPRRobustnessVersion;
        break;
    default:
        ChkArg( FALSE ); /* can't have any other cert type */
        break;
    }

    *f_pdwPlatform = g_dwPlatformID;

ErrorExit:
    return dr;
}

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_XBOX

/*****************************************************************************************************************
** Function:    Oem_Keyfile_LoadFromFile
**
** Synopsis:    Loads Keyfile from the disk to the Keyfile context.
**
** Arguments:   [f_pKeyFile]          : A pointer to the keyfile context that has been initialized
**                                      and the key file has already been opened.
*****************************************************************************************************************/
DRM_API DRM_RESULT Oem_Keyfile_LoadFromFile(
     __inout DRM_KEYFILE_CONTEXT *f_pKeyFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbKeyfileLen = 0;
    DRM_DWORD  cbRead = 0;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_GetSize( f_pKeyFile->hKeyfile, &cbKeyfileLen ), DRM_E_FILEREADERROR );
    ChkBOOL( cbKeyfileLen <= SIZEOF( f_pKeyFile->rgbKeyfileBuffer ), DRM_E_KEYFILE_TOO_LARGE );

    ChkBOOL( Oem_File_SetFilePointer( f_pKeyFile->hKeyfile, 0, OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );
    ChkBOOL( Oem_File_Read( f_pKeyFile->hKeyfile, f_pKeyFile->pbKeyfileBuffer, cbKeyfileLen, &cbRead ), DRM_E_FILEREADERROR );
    ChkBOOL( cbRead == cbKeyfileLen, DRM_E_FILEREADERROR );

ErrorExit:
    return dr;
}


/*****************************************************************************************************************
** Function:    Oem_Keyfile_StoreToFile
**
** Synopsis:    Stores Keyfile back to the disk.
**
** Arguments:   [f_pKeyFile]          : A pointer to the keyfile context that has been updated
**                                      and the key file has already been opened to write.
*****************************************************************************************************************/

DRM_API DRM_RESULT DRM_CALL Oem_Keyfile_StoreToFile(
     __inout DRM_KEYFILE_CONTEXT *f_pKeyFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbWritten = 0;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->cbKeyfileBuffer > 0 );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pKeyFile->fLoaded );

    ChkBOOL( Oem_File_SetFilePointer( f_pKeyFile->hKeyfile, 0, OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );
    ChkBOOL( Oem_File_Write( f_pKeyFile->hKeyfile, f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->cbKeyfileBuffer, &cbWritten ), DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == f_pKeyFile->cbKeyfileBuffer, DRM_E_FILEWRITEERROR );

    Oem_File_SetEndOfFile( f_pKeyFile->hKeyfile ); /* Intentionally ignore the result of this. The parser can handle the data not being truncated. */

    ChkBOOL( Oem_File_FlushBuffers( f_pKeyFile->hKeyfile ),  DRM_E_FILEWRITEERROR );

ErrorExit:
    return dr;
}

#endif

EXIT_PK_NAMESPACE_CODE;

