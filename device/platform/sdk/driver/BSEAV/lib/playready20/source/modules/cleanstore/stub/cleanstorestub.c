/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcleanstore.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_CLEANSTORE_RemoveUnusableLicenses(
    __in                                            DRM_BYTE                    *f_pContextLEVL,
    __in_bcount( SIZEOF( DRM_LICSTORE_CONTEXT ) )   DRM_BYTE                    *f_pContextLSTXML,
    __in_bcount( SIZEOF( DRM_LICSTORE_CONTEXT ) )   DRM_BYTE                    *f_pContextLSTXMR,
    __in                                            DRM_LICSTOREENUM_CONTEXT    *f_pLicStoreEnumContext,
    __in                                            DRM_DWORD                    f_dwFlags,
    __in_bcount(f_cbBuffer )                        DRM_BYTE                    *f_pbBuffer,
    __in                                            DRM_DWORD                    f_cbBuffer,
    __in                                      const DRM_VOID                    *f_pvCallerData,
    __in                                            DRM_DWORD                    f_dwCallbackInterval,
    __in                                            pfnStoreCleanupProgress      f_pfnCallback,
    __in                                            DRM_DST                     *f_pDatastore )
{
    UNREFERENCED_PARAMETER( f_pContextLEVL );
    UNREFERENCED_PARAMETER( f_pContextLSTXML );
    UNREFERENCED_PARAMETER( f_pContextLSTXMR );
    UNREFERENCED_PARAMETER( f_pLicStoreEnumContext );
    UNREFERENCED_PARAMETER( f_dwFlags );
    UNREFERENCED_PARAMETER( f_pbBuffer );
    UNREFERENCED_PARAMETER( f_cbBuffer );
    UNREFERENCED_PARAMETER( f_pvCallerData );
    UNREFERENCED_PARAMETER( f_dwCallbackInterval );
    UNREFERENCED_PARAMETER( f_pfnCallback );
    UNREFERENCED_PARAMETER( f_pDatastore );

    return DRM_SUCCESS;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

