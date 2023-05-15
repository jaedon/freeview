/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMDOMAINKEYXMRPARSER_C
#include <drmtypes.h>
#include <drmdomainkeyxmrparser.h>
#include <drmprofile.h>
#include <drmerr.h>
#include <byteorder.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function: DRM_DOMKEYXMR_GetHeader
**
** Synopsis: Parses an domain key XMR object header
**
** Arguments:
**
** [f_pbData]               -- Pointer to the XMR key data buffer
** [f_cbData]               -- size of the data buffer
** [f_pbOffset]             -- Pointer to the offset position in the buffer to start
** [f_poHeader]             -- Pointer to an header object structure
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetHeader(
   __in_bcount(f_cbData) const DRM_BYTE              *f_pbData,
   __in                        DRM_DWORD              f_cbData,
   __inout_ecount(1)           DRM_DWORD             *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_HEADER  *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetHeader );

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poHeader != NULL );
    ChkArg( f_cbData > *f_pbOffset + SIZEOF(DRM_DOMKEYXMR_HEADER) );

    dwOffset = *f_pbOffset;

    /*
    ** Read objects in the header
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->dwConstant,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->dwVersion,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->dwLength,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->dwSignedLength,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
** Function: DRM_DOMKEYXMR_GetSessionKey
**
** Synopsis: Parses an domain key XMR session key object
**
** Arguments:
**
** [f_pbData]               -- Pointer to the XMR key data buffer
** [f_cbData]               -- size of the data buffer
** [f_pbOffset]             -- Pointer to the offset position in the buffer to start
** [f_poKeyObj]             -- Pointer to an session key object structure
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetSessionKey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_SESSIONKEY  *f_poKeyObj)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetSessionKey );

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poKeyObj != NULL );
    ChkArg( f_cbData > *f_pbOffset + SIZEOF(DRM_DOMKEYXMR_SESSIONKEY) );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wFlags,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyObj->dwLength,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wKeyType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wEncryptionType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyObj->dwKeyLength,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    f_poKeyObj->pbData = f_pbData + dwOffset;

    dwOffset += f_poKeyObj->dwKeyLength;

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
** Function: DRM_DOMKEYXMR_GetPrivKeyContainer
**
** Synopsis: Parses an domain key XMR private key container object
**
** Arguments:
**
** [f_pbData]               -- Pointer to the XMR key data buffer
** [f_cbData]               -- size of the data buffer
** [f_pbOffset]             -- Pointer to the offset position in the buffer to start
** [f_poKeyContainer]       -- Pointer to an dromain private key container
**                             object structure
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivKeyContainer(
   __in_bcount(f_cbData) const DRM_BYTE                        *f_pbData,
   __in                        DRM_DWORD                        f_cbData,
   __inout_ecount(1)           DRM_DWORD                       *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_PRIVKEYCONTAINER  *f_poKeyContainer )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetPrivKeyContainer );

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poKeyContainer != NULL );
    ChkArg( f_cbData > *f_pbOffset + SIZEOF(DRM_DOMKEYXMR_PRIVKEYCONTAINER ) );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyContainer->wFlags,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyContainer->wType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyContainer->dwLength,
                                      f_pbData,
                                      dwOffset,
                                      f_cbData );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
**
** Function: DRM_DOMKEYXMR_GetPrivkey
**
** Synopsis: Parses an domain key XMR private key object
**
** Arguments:
**
** [f_pbData]               -- Pointer to the XMR key data buffer
** [f_cbData]               -- size of the data buffer
** [f_pbOffset]             -- Pointer to the offset position in the buffer to start
** [f_poKeyObj]             -- Pointer to an domain private key object structure
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivkey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_PRIVKEY *f_poKeyObj)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetPrivKey );

    ChkArg( f_pbData   != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poKeyObj != NULL );
    ChkArg( f_cbData > *f_pbOffset + SIZEOF(DRM_DOMKEYXMR_PRIVKEY) );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wFlags,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyObj->dwLength,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyObj->dwRevision,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wKeyType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poKeyObj->wEncryptionType,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poKeyObj->dwKeyLength,
                                     f_pbData,
                                     dwOffset,
                                     f_cbData );

    f_poKeyObj->pbData = f_pbData += dwOffset;

    dwOffset += f_poKeyObj->dwKeyLength;

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
