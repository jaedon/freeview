/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMINFO_C
#include <oemcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#ifdef PRDY_ROBUSTNESS
#include "../common/inc/oem_config.h"
#endif

const DRM_EXPORT_VAR DRM_WCHAR g_wchPathSeparator = WCHAR_CAST('/');
const DRM_EXPORT_VAR DRM_CHAR  g_chPathSeparator  = '/';

/* ./ */
static const DRM_WCHAR g_rgwchDrmPath [] =
{
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('b', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('m', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('v', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('\0', '\0')
};

/* The DRM path may be overridden by the test code */
DRM_EXPORT_VAR DRM_CONST_STRING g_dstrDrmPath   = CREATE_DRM_STRING( g_rgwchDrmPath );

#ifndef PRDY_ROBUSTNESS
/* priv.dat */
static const DRM_WCHAR g_rgwchPrivKey [] =
{
    ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0')
};

const DRM_CONST_STRING g_dstrPrivKey = CREATE_DRM_STRING( g_rgwchPrivKey );
#endif

DRM_BOOL g_fDeviceResetState = TRUE;

#ifdef PRDY_ROBUSTNESS
DRM_API DRM_RESULT DRM_CALL _GetDRMFullPathName( 
    __out DRM_WCHAR **ppwszFullPath, 
    __in_opt const DRM_CONST_STRING *pdstrFilename,
    __in DRMFILESPATH eFilesPath )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cchFullPath = 0;
    DRM_DWORD  cchPath;

    DRM_CONST_STRING path;

    ChkArg( ppwszFullPath != NULL );
    ChkDRMString( pdstrFilename );

    switch(eFilesPath){
        case CERTPATH:
            path.pwszString = bdrm_get_cert_dir();
            path.cchString  = bdrm_get_cert_dir_lgth();
            break;
        case CERTTEMPLATEPATH:
            path.pwszString = bdrm_get_cert_templates_dir();
            path.cchString  = bdrm_get_cert_templates_dir_lgth();
            break;
        case HDSPATH:
            path.pwszString = bdrm_get_hds_dir();
            path.cchString  = bdrm_get_hds_dir_lgth();
            break;
        default:
#if 0
            ChkDR(DRM_E_FILENOTFOUND);
#endif
			path.pwszString = bdrm_get_hds_dir();
            path.cchString  = bdrm_get_hds_dir_lgth();
            break;
    }

    cchPath = path.cchString;

    *ppwszFullPath = NULL;
    cchFullPath    = pdstrFilename->cchString 
                   + path.cchString 
                   + 2;

    if( pdstrFilename->cchString + path.cchString     < pdstrFilename->cchString
     || pdstrFilename->cchString + path.cchString + 2 < pdstrFilename->cchString
     || cchFullPath > cchFullPath * SIZEOF( DRM_WCHAR ) )
    {
        ChkDR( DRM_E_ARITHMETIC_OVERFLOW );
    }
    
    ChkMem( *ppwszFullPath = ( DRM_WCHAR * )Oem_MemAlloc( cchFullPath * SIZEOF( DRM_WCHAR ) ) );
    ChkDR( DRM_STR_StringCchCopyNW( *ppwszFullPath, cchFullPath, path.pwszString, path.cchString + 1 ) );

    /* Need to check whether it is ended with a backslash "\" */
    if ( (*ppwszFullPath)[ cchPath - 1 ] != g_wchPathSeparator )
    {
		#if 0
        (*ppwszFullPath)[ cchPath ] = g_wchPathSeperator;
        cchPath++;
		#endif
    }

    ChkDR( DRM_STR_StringCchCopyNW( *ppwszFullPath + cchPath, 
                               cchFullPath - cchPath, 
                               pdstrFilename->pwszString, 
                               pdstrFilename->cchString + 1 ) );
ErrorExit:
    if(dr != DRM_SUCCESS){
        if(*ppwszFullPath  != NULL) Oem_MemFree(*ppwszFullPath);
            *ppwszFullPath = NULL;
    }
    return dr;
}


/*****************************************************************************
** Function:    _GetPrivateKey
**
** Synopsis:    Obtains either the Group Certificate (GC) private key, or 
**              the Fallback signing key from the privkey file.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_fGCprivkey]  : TRUE for GC privkey, FALSE for Fallback signing key
**              [f_pbPrivkey]   : Returned privkey
**              [f_cbPrivKey]   : Size of the key buffer
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_PRIVKEYREADERROR - on any failure
**
** Notes:       The GC privkey and Fallback privkey are in the same file; the
**              GC privkey is the first 20 bytes, the Fallback signing key is the second 20 bytes.
**
******************************************************************************/
DRM_RESULT _GetPrivateKey(
    __in_opt DRM_VOID *f_pOEMContext,
    __in const DRM_BOOL f_fGCprivkey,
    __out_bcount( f_cbPrivKey ) DRM_BYTE *f_pbPrivkey,
    __in DRM_DWORD f_cbPrivKey )
{
	return BCM_GetPrivateKey(f_pOEMContext, f_fGCprivkey, f_pbPrivkey, f_cbPrivKey);
}

DRM_RESULT _GetHWID(DRM_BYTE *SerialNumber, DRM_BYTE *shaOut )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT sha_ctx;
    
    ChkArg( shaOut != NULL );
    ChkArg( SerialNumber != NULL );


    ChkDR(DRM_SHA_Init(&sha_ctx, eDRM_SHA_1));
    ChkDR(DRM_SHA_Update(SerialNumber, 16, &sha_ctx));
    ChkDR(DRM_SHA_Finalize(&sha_ctx, 20, shaOut));
    
ErrorExit:
    
    return dr;
}
#else
DRM_API DRM_RESULT DRM_CALL _GetDRMFullPathName(
    __out DRM_WCHAR **ppwszFullPath,
    __in_opt const DRM_CONST_STRING *pdstrFilename,
    __in DRMFILESPATH eFilesPath )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cchFullPath = 0;
    DRM_DWORD  cchPath     = g_dstrDrmPath.cchString;

    ChkArg( ppwszFullPath != NULL );

    ChkDRMString( pdstrFilename );

    *ppwszFullPath = NULL;
    cchFullPath    = pdstrFilename->cchString
                   + g_dstrDrmPath.cchString
                   + 2;

    if( pdstrFilename->cchString + g_dstrDrmPath.cchString     < pdstrFilename->cchString
     || pdstrFilename->cchString + g_dstrDrmPath.cchString + 2 < pdstrFilename->cchString
     || cchFullPath > cchFullPath * SIZEOF( DRM_WCHAR ) )
    {
        ChkDR( DRM_E_ARITHMETIC_OVERFLOW );
    }

    ChkMem( *ppwszFullPath = ( DRM_WCHAR * )Oem_MemAlloc( cchFullPath * SIZEOF( DRM_WCHAR ) ) );
    ChkDR( DRM_STR_StringCchCopyNW( *ppwszFullPath, cchFullPath, g_dstrDrmPath.pwszString, g_dstrDrmPath.cchString + 1 ) );

    /* Need to check whether it is ended with a backslash "\" */
    if ( (*ppwszFullPath)[ cchPath - 1 ] != g_wchPathSeparator )
    {
		#if 0

        (*ppwszFullPath)[ cchPath ] = g_wchPathSeparator;
        cchPath++;
		#endif

    }

    ChkDR( DRM_STR_StringCchCopyNW( *ppwszFullPath + cchPath,
                               cchFullPath - cchPath,
                               pdstrFilename->pwszString,
                               pdstrFilename->cchString + 1 ) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _GetPrivateKey
**
** Synopsis:    Obtains either the Group Certificate (GC) private key, or
**              the Fallback signing key from the privkey file.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_fGCprivkey]  : TRUE for GC privkey, FALSE for Fallback signing key
**              [f_pbPrivkey]   : Returned privkey
**              [f_cbPrivKey]   : Size of the key buffer
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_PRIVKEY_READ_ERROR - on any failure
**
** Notes:       The GC privkey and Fallback privkey are in the same file; the
**              GC privkey is the first 20 bytes, the Fallback signing key is the second 20 bytes.
**
******************************************************************************/
DRM_RESULT _GetPrivateKey(
    __in_opt DRM_VOID *f_pOEMContext,
    __in const DRM_BOOL f_fGCprivkey,
    __out_bcount( f_cbPrivKey ) DRM_BYTE *f_pbPrivkey,
    __in DRM_DWORD f_cbPrivKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR *pwszPath = NULL;
    DRM_DWORD iRead = 0;
    DRM_DWORD cbFile = 0;

    ChkArg( f_pbPrivkey != NULL );
    ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrPrivKey, CERTPATH ) );

    fp = Oem_File_Open( f_pOEMContext,
                        pwszPath,
                        OEM_GENERIC_READ,
                        OEM_FILE_SHARE_READ,
                        OEM_OPEN_EXISTING,
                        OEM_ATTRIBUTE_HIDDEN );

    if ( fp == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

    /* load file content in memory */
    if ( !Oem_File_GetSize( fp, &cbFile ) )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    if ( cbFile != ( 2 * SIZEOF( PRIVKEY ) ) )
    {
        ChkDR( DRM_E_PRIVKEY_READ_ERROR );
    }

    if ( !Oem_File_SetFilePointer( fp,
                                   ( f_fGCprivkey? 0 : SIZEOF( PRIVKEY ) ),
                                   OEM_FILE_BEGIN,
                                   NULL) )
    {
        ChkDR( DRM_E_FILE_SEEK_ERROR );
    }

    ChkBOOL( f_cbPrivKey >= SIZEOF( PRIVKEY ), DRM_E_BUFFERTOOSMALL );

    if ( !Oem_File_Read( fp, f_pbPrivkey, SIZEOF( PRIVKEY ), &iRead ) ||
         iRead != SIZEOF( PRIVKEY ) )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

ErrorExit:

    if( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }

    SAFE_OEM_FREE( pwszPath );

    return dr;
}

DRM_EXPORT_VAR DRM_BYTE g_rgbHWID[DRM_SHA1_DIGEST_LEN] = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
DRM_RESULT _GetHWID(
    __out_bcount(DRM_SHA1_DIGEST_LEN) DRM_BYTE *shaOut )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( shaOut != NULL );

    MEMCPY( shaOut, g_rgbHWID, DRM_SHA1_DIGEST_LEN );

ErrorExit:
    return dr;
}
#endif
/* NOTE TO OEM: Full implementation required */
/**********************************************************************
** Function:    Oem_Clock_GetResetState
** Synopsis:    Get the Secure Clock Reset state.
** Arguments:   [pOEMContext] -- OEM implemented context
**              [pfReset]     -- Bool to get the state
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Clock_GetResetState(
    __in_opt DRM_VOID *pOEMContext,
    __out DRM_BOOL *pfReset )
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkArg( pfReset != NULL );

    *pfReset = g_fDeviceResetState;

ErrorExit:
    return dr;
}

/* NOTE TO OEM: Full implementation required */
/**********************************************************************
** Function:    Oem_Clock_SetResetState
** Synopsis:    Set the Secure Clock Reset state.
** Arguments:   [pOEMContext] -- OEM implemented context
**              [pfReset]     -- Bool to set the state
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Clock_SetResetState(
    __in_opt DRM_VOID *pOEMContext,
    __in DRM_BOOL fReset )
{
    DRM_RESULT dr=DRM_SUCCESS;

    g_fDeviceResetState = fReset;

    return dr;
}

/*******************************************************************
**
**  Function:   Oem_Device_GetModelInfo
**
**  Synopsis:   Get the Model Info string for the Device Cert.
**
**  Arguments:  [pOEMContext]     -- OEM implemented context information
**              [pdstrModelInfo]  -- DRM_STRING pointer to hold the Model Info.
**                                   pdstrModelInfo->cchString should specify the size in characters
**                                   of pdstrModelInfo->pwszString.
**
**  Notes:  This string will be included in the Device Cert
**          (for WMDRM-PD and PlayReady certs). The resulting string will be null terminated.
**
**  Returns: DRM_E_BUFFERTOOSMALL   - If pdstrModelInfo->pwszString is too small to hold the string.
**           DRM_E_INVALIDARG       - If pdstrModelInfo or pdstrModelInfo->pwszString is NULL.
**           DRM_E_NOTIMPL          - If the model info is not supported for inclusion in the device cert on the platform
**           DRM_SUCCESS            - Otherwise.
**
*******************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Device_GetModelInfo(
    __in_opt DRM_VOID   *f_pOEMContext,
    __out_ecount_opt(*f_pcchModelInfo) DRM_WCHAR *f_pwchModelInfo,
    __inout DRM_DWORD *f_pcchModelInfo )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Config_Read(
    __in_opt DRM_VOID *f_pOEMContext,
    __in_z DRM_CHAR *f_szKey,
    __out_bcount_opt( *f_pcbValue ) DRM_BYTE *f_pbValue,
    __inout DRM_DWORD *f_pcbValue )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Config_Write(
    __in_opt DRM_VOID *f_pOEMContext,
    __in_z DRM_CHAR *f_szKey,
    __in_bcount( f_cbValue ) DRM_BYTE *f_pbValue,
    __in DRM_DWORD f_cbValue )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;
