/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDMANAGER_C
#include <drmmanager.h>
#include <drmcontract.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_GetMessageType(
    __inout_bcount( f_cbUnknownMessage )                        const DRM_BYTE                                         *f_pbUnknownMessage,
    __in                                                              DRM_DWORD                                         f_cbUnknownMessage,
    __out                                                             DRM_PRND_MESSAGE_TYPE                            *f_peMessageType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( DRM_PRND_TxRx_GetMessageType( f_pbUnknownMessage, f_cbUnknownMessage, f_peMessageType ) );
ErrorExit:
    ChkECC( ECC_Drm_Prnd_GetMessageType, dr );
    return dr;
}

DRM_API DRM_BYTE* DRM_CALL Drm_Prnd_MemAlloc(
    __in                                                              DRM_DWORD                                         f_cbToAlloc )
{
    if( f_cbToAlloc > 0 )
    {
        /* This API *must* be implemented using Oem_MemAlloc.  The PRND implementation requires it. */
        return (DRM_BYTE*)Oem_MemAlloc( f_cbToAlloc );
    }
    else
    {
        return NULL;
    }
}

DRM_API DRM_VOID DRM_CALL Drm_Prnd_MemFree(
    __in                                                              DRM_BYTE                                         *f_pbToFree )
{
    if( f_pbToFree != NULL )
    {
        /* This API *must* be implemented using Oem_MemFree.  The PRND implementation requires it. */
        Oem_MemFree( f_pbToFree );
    }
}

EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;

