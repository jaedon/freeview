/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMINFO_COMMON_C
#include <drmcrt.h>
#include <drmbase64.h>
#include <oemeccp160.h>
#include <oemeccp256.h>
#include <drmactivation.h>
#include <drmlastinclude.h>

#ifdef PRDY_ROBUSTNESS
//#include "../ansi/inc/oem_brcm.h"
#include "../inc/oem_config.h"
//#include "nexus_random_number.h"
#endif

ENTER_PK_NAMESPACE_CODE;

#ifndef PRDY_ROBUSTNESS
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7

/* zgpriv.dat */
static const DRM_WCHAR g_rgwchZGPrivKey [] =
{
    ONE_WCHAR('z', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),
    ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* ndrpriv.dat */
static const DRM_WCHAR g_rgwchNDRPrivKey [] =
{
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('p', '\0'),
    ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* ndrgpriv.dat - group (model) private key for ndr */
static const DRM_WCHAR g_rgwchNDRGPrivKey [] =
{
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('g', '\0'),
    ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0')
};

static const DRM_CONST_STRING g_dstrZGPrivKey = CREATE_DRM_STRING( g_rgwchZGPrivKey );
static const DRM_CONST_STRING g_dstrNDRPrivKey = CREATE_DRM_STRING( g_rgwchNDRPrivKey );
static const DRM_CONST_STRING g_dstrNDRGPrivKey = CREATE_DRM_STRING( g_rgwchNDRGPrivKey );

/*
**  XML format device certificate chain files
*/

/* devcerttemplate.dat */
static const DRM_WCHAR g_rgwchDevcerttemplate [] =
{
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

extern const DRM_CONST_STRING g_dstrDevcerttemplate = CREATE_DRM_STRING( g_rgwchDevcerttemplate );

/*
**  Binary format device certificate chain files
*/

/* bgroupcert.dat */
static const DRM_WCHAR g_rgwchZGroupCert [] =
{
    ONE_WCHAR('b', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('o', '\0'),
    ONE_WCHAR('u', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('e', '\0'),
    ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

static const DRM_CONST_STRING g_dstrZGroupCert = CREATE_DRM_STRING( g_rgwchZGroupCert );

/*
**  ND Receiver certificate chain files
*/

/* ndrcert.dat */
static const DRM_WCHAR g_rgwchReceiverCert [] =
{
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* ndrcerttemplate.dat */
static const DRM_WCHAR g_rgwchReceiverTemplate [] =
{
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

static const DRM_CONST_STRING g_dstrReceiverCert     = CREATE_DRM_STRING( g_rgwchReceiverCert );
static const DRM_CONST_STRING g_dstrReceiverTemplate = CREATE_DRM_STRING( g_rgwchReceiverTemplate );

#endif /* !DRM_BUILD_PROFILE_WM7 */

static const DRM_BYTE _rgbSecStorePassword [] =
{
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};
#endif
#define DEV_UNIQUE_ID_LENGTH	16
static uint8_t g_DeviceUniqueID[DEV_UNIQUE_ID_LENGTH] = {'u', 'n', 'i', 'q', '_', 'i', 'd','\0',};

#ifdef TEE
/* tee_state.dat */
static const DRM_WCHAR g_rgwchTeeState [ ] = {
    ONE_WCHAR('t', '\0'), ONE_WCHAR('e',  '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('_', '\0'),
    ONE_WCHAR('s', '\0'), ONE_WCHAR('t',  '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('.',  '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
    ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
};
static const DRM_CONST_STRING g_dstrTeeState = CREATE_DRM_STRING( g_rgwchTeeState );
/* tee_state.dat.tmp */
static const DRM_WCHAR g_rgwchTeeStateTmp [ ] = {
    ONE_WCHAR('t', '\0'), ONE_WCHAR('e',  '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('_', '\0'),
    ONE_WCHAR('s', '\0'), ONE_WCHAR('t',  '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('.',  '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
    ONE_WCHAR('t', '\0'), ONE_WCHAR('.',  '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('m', '\0'),
    ONE_WCHAR('p', '\0'), ONE_WCHAR('\0', '\0')
};
static const DRM_CONST_STRING g_dstrTeeStateTmp = CREATE_DRM_STRING( g_rgwchTeeStateTmp );
#endif

/*****************************************************************************
** Function:    _ReadFromFile
**
** Synopsis:    Reads the file specified into the provided buffer.
**              If this function is called with a NULL buffer pointer,
**              then the buffer size parm is updated with the required size (in bytes).
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_pbBuffer]    : Buffer pointer in which to store the file contents; may be NULL
**              [f_pcbBuffer]   : A pointer to the buffer size (in bytes); may be updated
**              [f_fFailOnTooLarge]: If TRUE the function will fail if the buffer supplied is larger than the
**                                file specified, if FALSE the function will read in only as much as will fit.
**              [f_pwszPath]    : The full path to the file to read
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_FILE_READ_ERROR  - if failure to read the file
**              DRM_E_FILE_SEEK_ERROR  - if failure to seek to the beginning of the file
**              DRM_E_BUFFERTOOSMALL - if the supplied buffer is NULL or too small
**              DRM_E_FILENOTFOUND   - if the specified file could not be found
**              DRM_E_INVALIDARG     - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _ReadFromFile(
    __in_opt                         DRM_VOID  *f_pOEMContext,
    __out_bcount_opt( *f_pcbBuffer ) DRM_BYTE  *f_pbBuffer,
    __inout                          DRM_DWORD *f_pcbBuffer,
    __in                             DRM_BOOL   f_fFailOnTooLarge,
    __in                       const DRM_WCHAR *f_pwszPath )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    OEM_FILEHDL hfp    = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbRead = 0;
    DRM_DWORD   cbFile = 0;


    ChkArg( f_pcbBuffer != NULL );
    ChkArg( f_pwszPath  != NULL );

    hfp = Oem_File_Open( f_pOEMContext,
                        f_pwszPath,
                        OEM_GENERIC_READ,
                        OEM_FILE_SHARE_READ,
                        OEM_OPEN_EXISTING,
                        OEM_ATTRIBUTE_HIDDEN );

    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_GetSize( hfp, &cbFile ) )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        if ( ( f_fFailOnTooLarge && *f_pcbBuffer < cbFile ) || ( f_pbBuffer == NULL ) )
        {
            *f_pcbBuffer = cbFile;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        if ( !Oem_File_SetFilePointer( hfp, 0, OEM_FILE_BEGIN, NULL ) )
        {
            ChkDR( DRM_E_FILE_SEEK_ERROR );
        }

        if( *f_pcbBuffer > cbFile )
        {
            *f_pcbBuffer = cbFile;
        }
        if (!Oem_File_Read( hfp, f_pbBuffer, *f_pcbBuffer, &cbRead ) || cbRead != *f_pcbBuffer )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }

    return dr;
}

#ifndef PRDY_ROBUSTNESS
/*****************************************************************************
** Function:    Oem_Device_GetKey
**
** Synopsis:    Obtains the specified private key.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_eKeyType]    : The type of key to obtain
**              [f_pbKey]       : A pointer to the buffer in which to put the key
**              [f_pcbKey]      : The size of the key buffer (in bytes); may be updated
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_BUFFERTOOSMALL   - if the provided buffer is NULL or too small
**              DRM_E_PRIVKEY_READ_ERROR - on any other error
**
** Notes:       For PlayReady model/group key, Oem_Device_GetKey() will not return the
**              actual key if the key is embedded. Instead Oem_Device_GetKey() will
**              return a all-0 key to indicate this special case. So the assumption is
**              that a valid PlayReady model/group key will never be a all-0 key.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetKey(
    __in_opt         DRM_VOID              *f_pOEMContext,
    __in      const  DRM_DEVICE_KEY_TYPE    f_eKeyType,
    __out_bcount_opt( *f_pcbKey ) DRM_BYTE *f_pbKey,
    __inout          DRM_DWORD             *f_pcbKey
)
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_WCHAR *pwszPath = NULL;

    ChkArg( f_pcbKey != NULL );

    switch( f_eKeyType )
    {
        case DRM_DKT_WMDRMPD_GROUP:
        {
            ChkArg( *f_pcbKey == SIZEOF( PRIVKEY ) );
            ChkDR( _GetPrivateKey( f_pOEMContext, TRUE, f_pbKey, *f_pcbKey ) );
            break;
        }

        case DRM_DKT_WMDRMPD_FALLBACK:
        {
            ChkArg( *f_pcbKey == SIZEOF( PRIVKEY ) );
            ChkDR( _GetPrivateKey( f_pOEMContext, FALSE, f_pbKey, *f_pcbKey ) );
            break;
        }
        case DRM_DKT_PLAYREADY_MODEL:
        {
            DRM_DEVICE_CERT_SOURCE eCertSource = DRM_DCT_SOURCE_FILE;

#if DRM_SUPPORT_EMBEDDED_CERTS
            /* Check whether the PlayReady group/model cert is embedded. */
            ChkDR( _GetDeviceCertSource( DRM_DCT_PLAYREADY_TEMPLATE, &eCertSource ) );
#endif /* DRM_SUPPORT_EMBEDDED_CERTS */

            if ( eCertSource == DRM_DCT_SOURCE_DLL_RESOURCE )
            {
                /*
                ** If the PlayReady group/model cert is embedded, the
                ** PlayReady group/model key must also be embedded.
                ** Return an all-0 key to indicate the real key
                ** is embedded.
                */
                ChkArg( *f_pcbKey == SIZEOF( PRIVKEY_P256 ) );
                OEM_ECC_ZeroPrivateKey_P256( (PRIVKEY_P256*)f_pbKey );
            }
            else
            {
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7
                ChkDR( _GetDRMFullPathName( &pwszPath, &g_dstrZGPrivKey, CERTTEMPLATEPATH) );
                ChkDR( _ReadFromFile( f_pOEMContext, f_pbKey, f_pcbKey, TRUE, pwszPath ) );
#else
                ChkDR( DRM_E_NOTIMPL );
#endif
            }
            break;
        }

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7

        case DRM_DKT_PLAYREADY_NDR:
        {
            ChkDR( _GetDRMFullPathName( &pwszPath, &g_dstrNDRPrivKey, CERTPATH) );
            ChkDR( _ReadFromFile( f_pOEMContext, f_pbKey, f_pcbKey, FALSE, pwszPath ) );
            break;
        }

        case DRM_DKT_PLAYREADY_NDR_MODEL:
        {
            ChkDR( _GetDRMFullPathName( &pwszPath, &g_dstrNDRGPrivKey, CERTTEMPLATEPATH) );
            ChkDR( _ReadFromFile( f_pOEMContext, f_pbKey, f_pcbKey, TRUE, pwszPath ) );
            break;
        }

#endif /* DRM_BUILD_PROFILE_WM7 */


        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

 ErrorExit:
    SAFE_OEM_FREE( pwszPath );

    if ( DRM_FAILED ( dr ) && dr != DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_PRIVKEY_READ_ERROR;
    }

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_SetKey
**
** Synopsis:    Saves the specified private key.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_eKeyType]    : The type of key to set
**              [f_pbKey]       : A pointer to a buffer containing the key
**              [f_cbKey]       : The size of the key buffer (in bytes)
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Currently, only playready and ndr devices save their private key.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_SetKey(
    __in_opt                     DRM_VOID            *f_pOEMContext,
    __in                  const  DRM_DEVICE_KEY_TYPE  f_eKeyType,
    __in_bcount(f_cbKey)         DRM_BYTE            *f_pbKey,
    __in                  const  DRM_DWORD            f_cbKey
)
{
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
        return DRM_E_NOTIMPL;
#else
    DRM_RESULT   dr        = DRM_SUCCESS;
    OEM_FILEHDL  hfp       = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD    cbWritten = 0;
    DRM_WCHAR   *pwszPath  = NULL;

    ChkArg( f_pbKey != NULL );
    ChkArg( f_cbKey != 0    );

    switch( f_eKeyType )
    {
        case DRM_DKT_PLAYREADY_NDR:
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrNDRPrivKey, CERTPATH ) );
            break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }

    hfp = Oem_File_Open( f_pOEMContext,
                         pwszPath,
                         OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                         OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                         OEM_CREATE_ALWAYS,
                         OEM_ATTRIBUTE_HIDDEN );

    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_Write( hfp, ( DRM_BYTE * )f_pbKey, f_cbKey, &cbWritten ) ||
             cbWritten != f_cbKey )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

 ErrorExit:
    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }

    SAFE_OEM_FREE( pwszPath );
    return dr;
#endif /* DRM_BUILD_PROFILE_WM7 */
}

/*****************************************************************************
** Function:    Oem_Device_GetCert
**
** Synopsis:    Obtains the specified device certificate chain.
**              If this function is called with a NULL cert chain buffer pointer,
**              then the buffer size parm is updated with the required size (in bytes).
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eCertType]   : The type of certificate chain to get
**              [f_pbDevCert]   : A pointer to a buffer in which to store the cert chain; may be NULL
**              [f_pcbDevCert]  : A pointer to cert chain buffer size (in bytes); may be updated
**
** Returns:     DRM_SUCCESS                    - on success
**              DRM_E_DEVCERT_READ_ERROR         - if failure to get the device cert chain
**              DRM_E_DEVCERT_TEMPLATE_READ_ERROR - if failure to get the device cert template
**              DRM_E_BUFFERTOOSMALL           - if the supplied buffer is NULL or too small
**              DRM_E_INVALIDARG               - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetCert(
    __in_opt                          DRM_VOID             *f_pOEMContext,
    __in                      const   DRM_DEVICE_CERT_TYPE  f_eCertType,
    __out_bcount_opt( *f_pcbDevCert ) DRM_BYTE             *f_pbDevCert,
    __inout                           DRM_DWORD            *f_pcbDevCert
)
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    DRM_RESULT              drReadError = DRM_SUCCESS;
    DRM_WCHAR              *pwszPath    = NULL;
    DRM_DEVICE_CERT_SOURCE  eCertSource = DRM_DCT_SOURCE_FILE;


#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7

    switch ( f_eCertType )
    {
        case DRM_DCT_WMDRMPD_TEMPLATE:
        {
            drReadError = DRM_E_DEVCERT_TEMPLATE_READ_ERROR;
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrDevcerttemplate, CERTTEMPLATEPATH ) );
            break;
        }

        case DRM_DCT_NETRECEIVER:
        {
            drReadError = DRM_E_DEVCERT_READ_ERROR;
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrReceiverCert, CERTPATH ) );
            break;
        }


        case DRM_DCT_PLAYREADY_TEMPLATE:
        {
            AssertChkArg( !DRM_ACTIVATION_IsActivationSupported() );
            drReadError = DRM_E_DEVCERT_TEMPLATE_READ_ERROR;
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrZGroupCert, CERTTEMPLATEPATH ) );
            break;
        }

        case DRM_DCT_NETRECEIVER_TEMPLATE:
        {
            drReadError = DRM_E_DEVCERT_TEMPLATE_READ_ERROR;
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrReceiverTemplate, CERTTEMPLATEPATH ) );
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

#endif  /* !DRM_BUILD_PROFILE_WM7 */


#if DRM_SUPPORT_EMBEDDED_CERTS
    ChkDR( _GetDeviceCertSource( f_eCertType, &eCertSource ) );
#endif

    if ( eCertSource == DRM_DCT_SOURCE_FILE )
    {
        ChkDR( _ReadFromFile( f_pOEMContext, f_pbDevCert, f_pcbDevCert, TRUE, pwszPath ) );
    }
    else    /* eCertSource == DRM_DCT_SOURCE_DLL_RESOURCE */
    {
#if DRM_SUPPORT_EMBEDDED_CERTS
        ChkDR( _ReadFromResource( f_eCertType, f_pbDevCert, f_pcbDevCert ) );
#else
        ChkDR( DRM_E_NOTIMPL );
#endif
    }

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        if( dr != DRM_E_BUFFERTOOSMALL && dr != DRM_E_INVALIDARG )
        {
            dr = drReadError;
        }
    }

    SAFE_OEM_FREE( pwszPath );
    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetSecureStoreGlobalPasswordSeed
**
** Synopsis:    Retreives the local device global secure store password.
**
** Arguments:   [f_rgbSecStorePassword] -- buffer to hold the secure store global password
**
** Returns:     DRM_SUCCESS on success. On Failure, it returns:
**              DRM_E_INVALIDARG
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetSecureStoreGlobalPasswordSeed(
	__in_opt DRM_VOID *f_pOEMContext,
    __out_bcount(DRM_SHA1_DIGEST_LEN) DRM_BYTE f_rgbSecStorePassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ]
)
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkArg( f_rgbSecStorePassword != NULL );

    MEMCPY( f_rgbSecStorePassword, _rgbSecStorePassword, DRM_SHA1_DIGEST_LEN );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_SetCert
**
** Synopsis:    Saves the specified device certificate chain.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eCertType]   : The type of certificate chain to save
**              [f_pbDevCert]   : A pointer to a buffer containing the cert chain
**              [f_cbDevCert]   : The cert chain buffer size (in bytes)
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if any parameter is invalid
**              DRM_E_FILE_WRITE_ERROR - if failure to save the dev cert chain
**              DRM_E_FILENOTFOUND   - if the file cannot be found
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_SetCert(
    __in_opt                        DRM_VOID             *f_pOEMContext,
    __in                     const  DRM_DEVICE_CERT_TYPE  f_eCertType,
    __in_bcount(f_cbDevCert)        DRM_BYTE             *f_pbDevCert,
    __in                     const  DRM_DWORD             f_cbDevCert )
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    OEM_FILEHDL  hfp       = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR   *pwszPath  = NULL;
    DRM_DWORD    cbWritten = 0;

    ChkArg( f_pbDevCert != NULL );
    ChkArg( f_cbDevCert != 0    );

    switch( f_eCertType )
    {
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WM7
        case DRM_DCT_PLAYREADY:
        case DRM_DCT_NETRECEIVER:
        {
            ChkDR(DRM_E_NOTIMPL);
            break;
        }
#else
        case DRM_DCT_NETRECEIVER:
        {
            ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrReceiverCert, CERTPATH ) );
            break;
        }
#endif /* DRM_BUILD_PROFILE_WM7 */
        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

    hfp = Oem_File_Open( f_pOEMContext,
                         pwszPath,
                         OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                         OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                         OEM_CREATE_ALWAYS,
                         OEM_ATTRIBUTE_HIDDEN );

    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_Write( hfp, ( DRM_BYTE * )f_pbDevCert, f_cbDevCert, &cbWritten ) ||
            cbWritten != f_cbDevCert )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }

    SAFE_OEM_FREE( pwszPath );

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetUniqueId
**
** Synopsis:    Obtains a unique identifier for the device.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eIDType]     : The type of ID to obtain
**              [f_pbUniqueId]  : A pointer to the buffer to receive the unique ID
**              [f_pcbLength]   : The length of the obtained unique ID (in bytes)
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if the length pointer is NULL
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Device_GetUniqueId(
    __in_opt                             DRM_VOID            *f_pOEMContext,
    __in                           const DRM_DEVICE_ID_TYPE   f_eIDType,
    __out_bcount_opt(*f_pcbLength)       DRM_BYTE            *f_pbUniqueId,
    __inout                              DRM_DWORD           *f_pcbLength )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE shaOut [ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ];

    ChkArg( f_pcbLength != NULL );

    OEM_SECURE_ZERO_MEMORY( shaOut, SIZEOF(shaOut) );
    switch ( f_eIDType )
    {
        case DRM_DIT_WMDRMPD_ID:
        {
            DRM_DWORD  dwSize = CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN ) * SIZEOF( DRM_WCHAR );

            if ( f_pbUniqueId == NULL || *f_pcbLength < dwSize )
            {
                *f_pcbLength = dwSize;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            ChkDR( _GetHWID( shaOut ) );

            dwSize = *f_pcbLength / SIZEOF( DRM_WCHAR );
            ChkDR( DRM_B64_EncodeW( shaOut, DRM_SHA1_DIGEST_LEN, (DRM_WCHAR *)f_pbUniqueId, &dwSize, 0 ) );

            *f_pcbLength = dwSize * SIZEOF( DRM_WCHAR );
            break;
        }

        case DRM_DIT_PLAYREADY_ID:
        {
            if ( ( f_pbUniqueId == NULL ) || ( *f_pcbLength < DRM_ID_SIZE ) )
            {
                *f_pcbLength = DRM_ID_SIZE;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            ChkDR( _GetHWID( shaOut ) );

            MEMCPY( f_pbUniqueId, shaOut, DRM_ID_SIZE );

            *f_pcbLength = DRM_ID_SIZE;
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

ErrorExit:
    return dr;
}

#else
#define GET_UINT32_FROM_BUF(pBuf) \
    (((uint32_t)(((uint8_t*)(pBuf))[0]) << 24) | \
     ((uint32_t)(((uint8_t*)(pBuf))[1]) << 16) | \
     ((uint32_t)(((uint8_t*)(pBuf))[2]) << 8)  | \
     ((uint8_t *)(pBuf))[3])

#define SET_UINT32_IN_BUF(pBuf, val) \
{   *pBuf       = (uint8_t)((val & 0xFF000000) >> 24);  \
    *(pBuf + 1) = (uint8_t)((val & 0xFF0000) >> 16);    \
    *(pBuf + 2) = (uint8_t)((val & 0xFF00) >> 8);       \
    *(pBuf + 3) = (uint8_t)((val & 0xFF));			    \
}

DRM_RESULT _Generate_Random_Number(DRM_BYTE *f_rgbSecStorePassword, DRM_DWORD  *f_pcbBuffer)
{
	return BCM_Generate_Random_Number(f_rgbSecStorePassword, f_pcbBuffer);
}

DRM_API DRM_RESULT _ReadAndDecryptFromFile (
    __in_opt     DRM_VOID  *f_pOEMContext,
    __in const   DRM_DEVICE_ID_TYPE   f_eIDType,
    __out        DRM_BYTE  *f_pbBuffer,
    __inout      DRM_DWORD *f_pcbBuffer,
    __in         DRM_WCHAR *f_pwszPath )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    OEM_FILEHDL hfp    = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbRead = 0;
    DRM_DWORD   cbFile = 0;
    int ii = 0;
    uint8_t* pDmaBuf = NULL;
    uint32_t data_size; // Size of decrypted data, without encryption padding (if any) and file header
    uint8_t *pKey = NULL;

    ChkArg( f_pcbBuffer != NULL );
    ChkArg( f_pwszPath  != NULL );

    hfp = Oem_File_Open( f_pOEMContext,
                        f_pwszPath,
                        OEM_GENERIC_READ,
                        OEM_FILE_SHARE_READ,
                        OEM_OPEN_EXISTING,
                        OEM_ATTRIBUTE_HIDDEN );

    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_GetSize( hfp, &cbFile ) )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        if( f_pbBuffer == NULL )
        {
            *f_pcbBuffer = cbFile;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        if ( !Oem_File_SetFilePointer( hfp, 0, OEM_FILE_BEGIN, NULL ) )
        {
            ChkDR( DRM_E_FILE_SEEK_ERROR );
        }

        pDmaBuf = Oem_MemHeapAlloc(cbFile);
        ChkMem(pDmaBuf);

        if (!Oem_File_Read( hfp, pDmaBuf, cbFile, &cbRead ) || cbRead != cbFile )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

		dr = BCM_DecryptFromFile(f_pOEMContext, f_eIDType, pDmaBuf, cbFile, pKey);
        ChkDR(dr);

        // The first 4 bytes of data in the file is the file header containing
        // the size of the encrypted data in bytes.
        // Recover size of the encrypted data from file.
        data_size = GET_UINT32_FROM_BUF(pDmaBuf);
        if ( ( *f_pcbBuffer < data_size ))
        {
            *f_pcbBuffer = data_size;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
        *f_pcbBuffer = data_size;

        MEMCPY(f_pbBuffer, &pDmaBuf[4], data_size);
    }
    else
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if(pDmaBuf != NULL)
        Oem_MemHeapFree(pDmaBuf);

    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }

    return dr;
}


DRM_API DRM_RESULT _EncryptAndWriteToFile (
    __in_opt     DRM_VOID  *f_pOEMContext,
    __in const   DRM_DEVICE_ID_TYPE   f_eIDType,
    __in         DRM_BYTE  *f_pbBuffer,
    __in         DRM_DWORD  f_pcbBuffer,
    __in         DRM_WCHAR *f_pwszPath
    )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    OEM_FILEHDL hfp    = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbRead = 0;
    uint8_t             *pDmaBuf = NULL;
    uint32_t             size;
    uint8_t             *pKey = NULL;
    DRM_DWORD            cbWritten = 0;

    ChkArg( f_pcbBuffer != NULL );
    ChkArg( f_pwszPath  != NULL );

    hfp = Oem_File_Open( f_pOEMContext,
                         f_pwszPath,
                         OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                         OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                         OEM_CREATE_ALWAYS,
                         OEM_ATTRIBUTE_HIDDEN );
    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        size =  f_pcbBuffer;
        // Keep 4 bytes as a file header to save the size of the plain text data segment
        size += 4;
        // Compute the amount of padding needed if any
        size += (16 - (size % 16));

        pDmaBuf = Oem_MemHeapAlloc(size);
        ChkMem(pDmaBuf);

        SET_UINT32_IN_BUF(pDmaBuf, f_pcbBuffer); // Save buffer size in the file header.
        MEMCPY(&pDmaBuf[4], f_pbBuffer, f_pcbBuffer);

		dr = BCM_EncryptToFile(f_pOEMContext, f_eIDType, pDmaBuf, pDmaBuf, size, pKey);
        ChkDR(dr);

        if ( !Oem_File_Write( hfp, ( DRM_BYTE * )pDmaBuf, size, &cbWritten ) ||
             cbWritten != size )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if(pDmaBuf != NULL)
        Oem_MemHeapFree(pDmaBuf);

    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }
    return dr;
}

DRM_RESULT _GetDTKFilename(
    __in const  DRM_DEVICE_KEY_TYPE  f_eKeyType,
    __in_opt    DRM_STRING *pdstrFilename
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ChkArg( pdstrFilename != NULL );

    switch( f_eKeyType )
    {
        case DRM_DKT_WMDRMPD_GROUP:
        case DRM_DKT_WMDRMPD_FALLBACK:
        case DRM_DKT_PLAYREADY_MODEL:
        case DRM_DKT_PLAYREADY_NDR_MODEL:
        {
            pdstrFilename->pwszString = NULL;
            pdstrFilename->cchString = 0;
            break;
        }

        case DRM_DKT_PLAYREADY_DEVICE_SIGN:
        {
            pdstrFilename->pwszString = bdrm_get_zgpriv_sig_key_fname();
            pdstrFilename->cchString = bdrm_get_zgpriv_sig_key_fname_lgth();
            break;
        }


        case DRM_DKT_PLAYREADY_DEVICE_ENCRYPT:
        {
            pdstrFilename->pwszString = bdrm_get_zgpriv_enc_key_fname();
            pdstrFilename->cchString = bdrm_get_zgpriv_enc_key_fname_lgth();

            break;
        }

        case DRM_DKT_PLAYREADY_NDR:
        {
            pdstrFilename->pwszString = bdrm_get_ndr_priv_key_fname();
            pdstrFilename->cchString = bdrm_get_ndr_priv_key_fname_lgth();
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT _GetDCTFilename(
    __in    const   DRM_DEVICE_CERT_TYPE  f_eCertType,
    __in_opt        DRM_STRING *pdstrFilename
)
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( pdstrFilename != NULL );

    switch ( f_eCertType )
    {
        case DRM_DCT_WMDRMPD:
        {
            pdstrFilename->pwszString = bdrm_get_dev_cert_fname();
            pdstrFilename->cchString = bdrm_get_dev_cert_fname_lgth();
            break;
        }

        case DRM_DCT_PLAYREADY:
        {
            pdstrFilename->pwszString = bdrm_get_zdev_cert_fname();
            pdstrFilename->cchString = bdrm_get_zdev_cert_fname_lgth();
            break;
        }

        case DRM_DCT_NETRECEIVER:
        {
            pdstrFilename->pwszString = bdrm_get_ndr_cert_fname();
            pdstrFilename->cchString = bdrm_get_ndr_cert_fname_lgth();
            break;
        }

        case DRM_DCT_NETRECEIVER_TEMPLATE:
        {
            pdstrFilename->pwszString = bdrm_get_ndr_cert_template_fname();
            pdstrFilename->cchString = bdrm_get_ndr_cert_template_fname_lgth();
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Oem_Device_GetKey
**
** Synopsis:    Obtains the specified private key.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_eKeyType]    : The type of key to obtain
**              [f_pbKey]       : A pointer to the buffer in which to put the key
**              [f_pcbKey]      : The size of the key buffer (in bytes); may be updated
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_BUFFERTOOSMALL   - if the provided buffer is NULL or too small
**              DRM_E_PRIVKEYREADERROR - on any other error
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetKey(
    __in_opt         DRM_VOID              *f_pOEMContext,
    __in      const  DRM_DEVICE_KEY_TYPE    f_eKeyType,
    __out_bcount_opt( *f_pcbKey ) DRM_BYTE *f_pbKey,
    __inout          DRM_DWORD             *f_pcbKey
)
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_WCHAR   *pwszPath    = NULL;
    DRM_STRING           filename;

    ChkArg( f_pcbKey != NULL );

    ChkDR(_GetDTKFilename(f_eKeyType, &filename));

    switch( f_eKeyType )
    {
        case DRM_DKT_WMDRMPD_GROUP:
        {
            ChkArg( *f_pcbKey == SIZEOF( PRIVKEY ) );
            ChkDR( _GetPrivateKey( f_pOEMContext, TRUE, f_pbKey, *f_pcbKey ) );
            break;
        }

        case DRM_DKT_WMDRMPD_FALLBACK:
        {
            ChkArg( *f_pcbKey == SIZEOF( PRIVKEY ) );
            ChkDR( _GetPrivateKey( f_pOEMContext, FALSE, f_pbKey, *f_pcbKey ) );
            break;
        }

        case DRM_DKT_PLAYREADY_MODEL:
        {
            ChkArg( *f_pcbKey == 32);
            BCM_GetPrPrivateKey(f_pOEMContext, f_eKeyType ,f_pbKey, *f_pcbKey );
            break;
        }

        case DRM_DKT_PLAYREADY_DEVICE_SIGN:
        {
            ChkDR( _GetDRMFullPathName( &pwszPath, &filename, CERTPATH) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbKey, f_pcbKey, pwszPath ) );
            break;
        }

        case DRM_DKT_PLAYREADY_DEVICE_ENCRYPT:
        {
            ChkDR( _GetDRMFullPathName( &pwszPath, &filename, CERTPATH) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbKey, f_pcbKey, pwszPath ) );
            break;
        }

        case DRM_DKT_PLAYREADY_NDR:
        {
            ChkDR( _GetDRMFullPathName( &pwszPath, &filename, CERTPATH) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbKey, f_pcbKey, pwszPath ) );
            break;
        }

        case DRM_DKT_PLAYREADY_NDR_MODEL:
        {
            ChkArg( *f_pcbKey == 32);
            BCM_GetPrPrivateKey(f_pOEMContext, f_eKeyType ,f_pbKey, *f_pcbKey );
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

 ErrorExit:
    SAFE_OEM_FREE( pwszPath );

    if ( DRM_FAILED ( dr ) && dr != DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_PRIVKEY_READ_ERROR;
    }

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_SetKey
**
** Synopsis:    Saves the specified private key.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**              [f_eKeyType]    : The type of key to set
**              [f_pbKey]       : A pointer to a buffer containing the key
**              [f_cbKey]       : The size of the key buffer (in bytes)
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_SetKey(
    __in_opt                     DRM_VOID            *f_pOEMContext,
    __in                  const  DRM_DEVICE_KEY_TYPE  f_eKeyType,
    __in_bcount(f_cbKey)         DRM_BYTE            *f_pbKey,
    __in                  const  DRM_DWORD            f_cbKey
)
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    OEM_FILEHDL  hfp       = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR   *pwszPath  = NULL;
    DRM_STRING     filename;

    ChkArg( f_pbKey != NULL );
    ChkArg( f_cbKey != 0    );
    /*
    ** Currently, it is only valid for a playready and ndr devices to save its private key
    */

    ChkDR(_GetDTKFilename(f_eKeyType, &filename));
    switch( f_eKeyType )
    {
        case DRM_DKT_PLAYREADY_DEVICE_SIGN:
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            break;
        case DRM_DKT_PLAYREADY_DEVICE_ENCRYPT:
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            break;
        case DRM_DKT_PLAYREADY_NDR:
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }

     ChkDR(  _EncryptAndWriteToFile (f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbKey,
                                     f_cbKey, pwszPath ));
 ErrorExit:

    SAFE_OEM_FREE( pwszPath );
    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetCert
**
** Synopsis:    Obtains the specified device certificate chain.
**              If this function is called with a NULL cert chain buffer pointer,
**              then the buffer size parm is updated with the required size (in bytes).
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eCertType]   : The type of certificate chain to get
**              [f_pbDevCert]   : A pointer to a buffer in which to store the cert chain; may be NULL
**              [f_pcbDevCert]  : A pointer to cert chain buffer size (in bytes); may be updated
**
** Returns:     DRM_SUCCESS                    - on success
**              DRM_E_DEVCERTREADERROR         - if failure to get the device cert chain
**              DRM_E_DEVCERTTEMPLATEREADERROR - if failure to get the device cert template
**              DRM_E_BUFFERTOOSMALL           - if the supplied buffer is NULL or too small
**              DRM_E_INVALIDARG               - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetCert(
    __in_opt        DRM_VOID             *f_pOEMContext,
    __in    const   DRM_DEVICE_CERT_TYPE  f_eCertType,
    __out_bcount_opt( *f_pcbDevCert ) DRM_BYTE *f_pbDevCert,
    __inout         DRM_DWORD            *f_pcbDevCert
)
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_RESULT   drReadError = DRM_SUCCESS;
    DRM_WCHAR   *pwszPath    = NULL;
    DRM_STRING  filename;

    switch ( f_eCertType )
    {
        case DRM_DCT_WMDRMPD:
        {
            drReadError = DRM_E_DEVCERT_READ_ERROR;
            ChkDR(_GetDCTFilename(f_eCertType, &filename));
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_WMDRMPD_ID, f_pbDevCert, f_pcbDevCert, pwszPath ));
            break;
        }

        case DRM_DCT_PLAYREADY:
        {
            drReadError = DRM_E_DEVCERT_READ_ERROR;
            ChkDR(_GetDCTFilename(f_eCertType, &filename));
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbDevCert, f_pcbDevCert, pwszPath ));
            break;
        }

        case DRM_DCT_NETRECEIVER:
        {
            drReadError = DRM_E_DEVCERT_READ_ERROR;
            ChkDR(_GetDCTFilename(f_eCertType, &filename));
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            ChkDR( _ReadAndDecryptFromFile( f_pOEMContext, DRM_DIT_PLAYREADY_ID, f_pbDevCert, f_pcbDevCert, pwszPath ));
            break;
        }

        case DRM_DCT_WMDRMPD_TEMPLATE:
        {
            if ( ( *f_pcbDevCert <  BCM_GetDevTemplateSize(f_pOEMContext)) || ( f_pbDevCert == NULL ) )
            {
                *f_pcbDevCert = BCM_GetDevTemplateSize(f_pOEMContext);
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
            else {
				uint8_t * pDevTemplate = BCM_GetDevTemplate(f_pOEMContext);
                MEMCPY(f_pbDevCert, pDevTemplate, BCM_GetDevTemplateSize(f_pOEMContext));
				*f_pcbDevCert = BCM_GetDevTemplateSize(f_pOEMContext);
            }
            break;
        }

        case DRM_DCT_PLAYREADY_TEMPLATE:
        {
            if ( ( *f_pcbDevCert <  BCM_GetZGroupCertSize(f_pOEMContext)) || ( f_pbDevCert == NULL ) )
            {
                *f_pcbDevCert = BCM_GetZGroupCertSize(f_pOEMContext);
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
            else {
				uint8_t * pZGroupCert = BCM_GetZGroupCert(f_pOEMContext);
                MEMCPY(f_pbDevCert, pZGroupCert, BCM_GetZGroupCertSize(f_pOEMContext));
				*f_pcbDevCert = BCM_GetZGroupCertSize(f_pOEMContext);
            }
            break;
        }

        case DRM_DCT_NETRECEIVER_TEMPLATE:
        {
            drReadError = DRM_E_DEVCERT_TEMPLATE_READ_ERROR;
            ChkDR(_GetDCTFilename(f_eCertType, &filename));
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTTEMPLATEPATH ) );
            ChkDR( _ReadFromFile( f_pOEMContext, f_pbDevCert, f_pcbDevCert, TRUE, pwszPath ) );
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }


ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        if( dr != DRM_E_BUFFERTOOSMALL && dr != DRM_E_INVALIDARG )
        {
            dr = drReadError;
        }
    }

    SAFE_OEM_FREE( pwszPath );
    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetRevocationStatus
**
** Synopsis:    Querry the revocation status of the keys.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context
**
** Returns:     DRM_SUCCESS on success. On Failure, it returns:
**              DRM_E_INVALIDARG
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetRevocationStatus(
    __in              DRM_VOID  *f_pOEMContext,
    __out             DRM_BOOL  *f_pStatus
)
{
    DRM_RESULT   dr = DRM_SUCCESS;

    ChkArg( f_pOEMContext != NULL );
    ChkArg( f_pStatus != NULL );

    *f_pStatus = BCM_Getrevoked(f_pOEMContext);

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_SaveRevocationInfo
**
** Synopsis:    Save revocation information.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context
**
** Returns:     DRM_SUCCESS on success. On Failure, it returns:
**              DRM_E_INVALIDARG
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_SaveRevocationInfo(
    __in              DRM_VOID  *f_pOEMContext
)
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_WCHAR   *pwszPath = NULL;
    DRM_STRING   filename;
    OEM_FILEHDL  hfp    = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE key[32];
    uint32_t key_size = 32;
    DRM_BYTE ref_key[32];

    ChkArg( f_pOEMContext != NULL );

    filename.pwszString = bdrm_get_revocation_fname();
    filename.cchString  = bdrm_get_revocation_fname_lgth();

    ChkDR(_GetDRMFullPathName(&pwszPath, &filename, HDSPATH));
    ChkDR(Oem_Device_GetKey(f_pOEMContext, DRM_DKT_PLAYREADY_MODEL, key, &key_size));

    hfp = Oem_File_Open( f_pOEMContext,
                       pwszPath,
                       OEM_GENERIC_READ,
                       OEM_FILE_SHARE_READ,
                       OEM_OPEN_EXISTING,
                       OEM_ATTRIBUTE_HIDDEN );
    if (hfp == OEM_INVALID_HANDLE_VALUE)
    {
        ChkDR(_EncryptAndWriteToFile (f_pOEMContext, DRM_DIT_PLAYREADY_ID, key, key_size, pwszPath));
		BCM_Setrevoked(f_pOEMContext, TRUE);
    }
    else {
        ChkDR(_ReadAndDecryptFromFile (f_pOEMContext, DRM_DIT_PLAYREADY_ID, ref_key, &key_size, pwszPath));
        if(BKNI_Memcmp(key, ref_key, 32) != 0){
			BCM_Setrevoked(f_pOEMContext, TRUE);

        }
        else {
			BCM_Setrevoked(f_pOEMContext, FALSE);
        }
    }

ErrorExit:
    BKNI_Memset(key, 0, 32);
    BKNI_Memset(ref_key, 0, 32);
    if(hfp != OEM_INVALID_HANDLE_VALUE) Oem_File_Close(hfp);
	if(pwszPath!=NULL)
	{
		SAFE_OEM_FREE(pwszPath);
	}

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetSecureStoreGlobalPasswordSeed
**
** Synopsis:    Retreives the local device global secure store password.
**
** Arguments:   [f_rgbSecStorePassword] -- buffer to hold the secure store global password
**
** Returns:     DRM_SUCCESS on success. On Failure, it returns:
**              DRM_E_INVALIDARG
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetSecureStoreGlobalPasswordSeed(
	__in_opt DRM_VOID *f_pOEMContext,
    __out_bcount(DRM_SHA1_DIGEST_LEN) DRM_BYTE f_rgbSecStorePassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ]
)
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkArg( f_rgbSecStorePassword != NULL );
    DRM_WCHAR   *pwszPath    = NULL;
    DRM_STRING     filename;
    DRM_DEVICE_ID_TYPE f_eIDType = DRM_DIT_PLAYREADY_ID;
    OEM_FILEHDL hfp    = OEM_INVALID_HANDLE_VALUE;

    filename.pwszString = bdrm_get_hds_pwd_fname();
    filename.cchString = bdrm_get_hds_pwd_fname_lgth();

    ChkDR(_GetDRMFullPathName(&pwszPath, &filename, HDSPATH));

    /* Get OEM context.*/

	if(BCM_Getpr_keys_valid(f_pOEMContext) == FALSE) {
	    if(BCM_Getwmdrm_keys_valid(f_pOEMContext) == TRUE)
                f_eIDType = DRM_DIT_WMDRMPD_ID;
	    else
            {
            ChkDR(DRM_E_CRYPTO_FAILED);
            }
         }

    /* check if file exist*/
    hfp = Oem_File_Open( f_pOEMContext,
                       pwszPath,
                       OEM_GENERIC_READ,
                       OEM_FILE_SHARE_READ,
                       OEM_OPEN_EXISTING,
                       OEM_ATTRIBUTE_HIDDEN );

    if (hfp == OEM_INVALID_HANDLE_VALUE)
    {
    	DRM_DWORD  f_pcbBuffer;

    	ChkDR(_Generate_Random_Number(f_rgbSecStorePassword, &f_pcbBuffer));
		ChkDR(_EncryptAndWriteToFile (f_pOEMContext, f_eIDType, f_rgbSecStorePassword, f_pcbBuffer, pwszPath));
    }
    else {
        DRM_DWORD size = DRM_SHA1_DIGEST_LEN;

        ChkDR(_ReadAndDecryptFromFile (f_pOEMContext, f_eIDType, f_rgbSecStorePassword, &size, pwszPath));
        if(size != DRM_SHA1_DIGEST_LEN){
            ChkDR(DRM_E_CRYPTO_FAILED);
        }
    }

ErrorExit:
    if(hfp != OEM_INVALID_HANDLE_VALUE) Oem_File_Close(hfp);

	if(pwszPath!=NULL)
	{
		SAFE_OEM_FREE(pwszPath);
	}

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_SetCert
**
** Synopsis:    Saves the specified device certificate chain.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eCertType]   : The type of certificate chain to save
**              [f_pbDevCert]   : A pointer to a buffer containing the cert chain
**              [f_cbDevCert]   : The cert chain buffer size (in bytes)
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if any parameter is invalid
**              DRM_E_FILEWRITEERROR - if failure to save the dev cert chain
**              DRM_E_FILENOTFOUND   - if the file cannot be found
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_SetCert(
    __in_opt                        DRM_VOID             *f_pOEMContext,
    __in                     const  DRM_DEVICE_CERT_TYPE  f_eCertType,
    __in_bcount(f_cbDevCert)        DRM_BYTE             *f_pbDevCert,
    __in                     const  DRM_DWORD             f_cbDevCert )
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    OEM_FILEHDL  hfp       = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR   *pwszPath  = NULL;
    DRM_DEVICE_ID_TYPE   f_eIDType = DRM_DIT_PLAYREADY_ID;

    DRM_STRING     filename;

    ChkArg( f_pbDevCert != NULL );
    ChkArg( f_cbDevCert != 0    );
    ChkDR(_GetDCTFilename(f_eCertType, &filename));
    switch( f_eCertType )
    {
        case DRM_DCT_WMDRMPD:
        {
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            f_eIDType = DRM_DIT_WMDRMPD_ID;
            break;
        }

        case DRM_DCT_PLAYREADY:
        {
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            break;
        }

        case DRM_DCT_NETRECEIVER:
        {
            ChkDR( _GetDRMFullPathName ( &pwszPath, &filename, CERTPATH ) );
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

     ChkDR(  _EncryptAndWriteToFile (f_pOEMContext, f_eIDType, f_pbDevCert, f_cbDevCert, pwszPath ));

ErrorExit:
    SAFE_OEM_FREE( pwszPath );

    return dr;
}

/*****************************************************************************
** Function:    Oem_Device_GetUniqueId
**
** Synopsis:    Obtains a unique identifier for the device.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eIDType]     : The type of ID to obtain
**              [f_pbUniqueId]  : A pointer to the buffer to receive the unique ID
**              [f_pcbLength]   : The length of the obtained unique ID (in bytes)
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if the length pointer is NULL
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Device_GetUniqueId(
    __in_opt                             DRM_VOID            *f_pOEMContext,
    __in                           const DRM_DEVICE_ID_TYPE   f_eIDType,
    __out_bcount_opt(*f_pcbLength)       DRM_BYTE            *f_pbUniqueId,
    __inout                              DRM_DWORD           *f_pcbLength )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE shaOut [ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = {0};

    ChkArg( f_pOEMContext != NULL );
    ChkArg( f_pcbLength != NULL );
	if(!strcmp(g_DeviceUniqueID,"uniq_id"))
	{
		ChkDR( BCM_GetChipUniqueID(g_DeviceUniqueID, DEV_UNIQUE_ID_LENGTH));
#if 0
		printf("[g_DeviceUniqueID] = ");
		for(i=0; i<DEV_UNIQUE_ID_LENGTH ; ++i){
			printf("%x ",g_DeviceUniqueID[i]);
		}
		printf("\nOutput end\n");
#endif
	}

    switch ( f_eIDType )
    {
        case DRM_DIT_WMDRMPD_ID:
        {
            DRM_DWORD  dwSize = CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN ) * SIZEOF( DRM_WCHAR );

            if ( f_pbUniqueId == NULL || *f_pcbLength < dwSize )
            {
                *f_pcbLength = dwSize;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
            _GetHWID((DRM_BYTE *)g_DeviceUniqueID, shaOut );

            dwSize = *f_pcbLength / SIZEOF( DRM_WCHAR );
            ChkDR( DRM_B64_EncodeW( shaOut, DRM_SHA1_DIGEST_LEN, (DRM_WCHAR *)f_pbUniqueId, &dwSize, 0 ) );

            *f_pcbLength = dwSize * SIZEOF( DRM_WCHAR );
#if 0
			printf("\nf_pcbLength = %d\n",*f_pcbLength);
			for(i=0; i< dwSize * SIZEOF( DRM_WCHAR ) ; ++i){
				printf("%x ",f_pbUniqueId[i]);
			}
			printf("\nOutput end\n");
#endif
            break;
        }

        case DRM_DIT_PLAYREADY_ID:
        {
            if ( ( f_pbUniqueId == NULL ) || ( *f_pcbLength < DRM_ID_SIZE ) )
            {
                *f_pcbLength = DRM_ID_SIZE;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
            _GetHWID((DRM_BYTE *)g_DeviceUniqueID, shaOut );

            MEMCPY( f_pbUniqueId, shaOut, DRM_ID_SIZE );
#if 0
			printf("[DRM_DIT_PLAYREADY_ID] RESULT  = ");
			for(i=0; i<DRM_ID_SIZE ; ++i){
				printf("%x ",f_pbUniqueId[i]);
			}
			printf("\nOutput end\n");
#endif
            *f_pcbLength = DRM_ID_SIZE;
            break;
        }

        default:
        {
            ChkDR( DRM_E_INVALIDARG );
            break;
        }
    }

ErrorExit:
    return dr;
}
#endif

#ifdef TEE
DRM_API DRM_RESULT DRM_CALL Oem_WriteTeeState(DRM_BYTE * const buf, const DRM_DWORD len) {
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_WCHAR   *pwszPath  = NULL;
    ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrTeeStateTmp, CERTTEMPLATEPATH ) );
    DRM_DWORD len1;
    OEM_FILEHDL hdl = Oem_File_Open(NULL, pwszPath, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL);
    if (hdl != OEM_INVALID_HANDLE_VALUE) {
        if ( !Oem_File_Write( hdl, buf, len, &len1 ) || len != len1) {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
        Oem_File_Close(hdl);
    } else {
        ChkDR( DRM_E_FILEOPEN );
    }

    DRM_WCHAR   *pwszPathTo  = NULL;
    ChkDR( _GetDRMFullPathName ( &pwszPathTo, &g_dstrTeeState, CERTTEMPLATEPATH ) );
    if ( !Oem_File_Rename( pwszPath, pwszPathTo) ) {
        ChkDR( DRM_E_FILE_WRITE_ERROR );
    }

ErrorExit:
    if (pwszPath != NULL) Oem_MemFree(pwszPath);
    if (pwszPathTo != NULL) Oem_MemFree(pwszPathTo);
    return dr;
}
DRM_API DRM_RESULT DRM_CALL Oem_ReadTeeState(DRM_BYTE * const buf, DRM_DWORD * len) {
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_WCHAR   *pwszPath  = NULL;
    ChkDR( _GetDRMFullPathName ( &pwszPath, &g_dstrTeeState, CERTTEMPLATEPATH ) );
    DRM_DWORD len1;
    OEM_FILEHDL hdl = Oem_File_Open(NULL, pwszPath, OEM_GENERIC_READ, OEM_FILE_SHARE_NONE, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if (hdl != OEM_INVALID_HANDLE_VALUE) {
        if ( !Oem_File_Read( hdl, buf, *len, &len1 )) {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }
        *len = len1;
        Oem_File_Close(hdl);
    } else {
        ChkDR( DRM_E_FILEOPEN );
    }

ErrorExit:
    if (pwszPath != NULL) Oem_MemFree(pwszPath);
    return dr;
}
#endif

EXIT_PK_NAMESPACE_CODE;
