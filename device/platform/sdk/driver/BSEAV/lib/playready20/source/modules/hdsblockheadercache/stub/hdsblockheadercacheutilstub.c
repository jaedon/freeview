/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmhdsblockheadercache.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_BOOL DRM_CALL DRM_HBHC_IsHdsBlockHeaderCacheSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_HBHC_IsHdsBlockHeaderCacheUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_RESULT DRM_CALL DRM_HBHC_GetBlockHeaderFromCache(
    __in     _NsContext     *f_pNS,
    __in     DRM_DWORD       f_nBlkNum,
    __inout  _CommBlockHDR **f_ppBlock,
    __in     DRM_BOOL        f_fGenericHeaderOnly,
    __out    DRM_BOOL       *f_pfFound )
{
    DRM_RESULT dr = DRM_SUCCESS;

    UNREFERENCED_PARAMETER( f_pNS );
    UNREFERENCED_PARAMETER( f_nBlkNum );
    UNREFERENCED_PARAMETER( f_ppBlock );
    UNREFERENCED_PARAMETER( f_fGenericHeaderOnly );

    ChkArg( f_pfFound != NULL );
    f_pfFound = FALSE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_HBHC_UpdateBlockHeaderCache(
    __in                                      DRM_DWORD   f_dwBlockNumber,
    __in_opt                                  OEM_FILEHDL f_hHDSFileHandle,
    __in                                      DRM_DWORD   f_dwBlockHeaderMetadata,
    __in_bcount_opt( f_cbBytesToWrite ) const DRM_BYTE   *f_pbBlock,
    __in                                      DRM_DWORD   f_cbBytesToWrite )
{
    UNREFERENCED_PARAMETER( f_dwBlockNumber );
    UNREFERENCED_PARAMETER( f_hHDSFileHandle );
    UNREFERENCED_PARAMETER( f_dwBlockHeaderMetadata );
    UNREFERENCED_PARAMETER( f_pbBlock );
    UNREFERENCED_PARAMETER( f_cbBytesToWrite );
    return DRM_SUCCESS;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

