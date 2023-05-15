/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmest.h>
#include <drmembeddedstore_impl.h>
#include <drmresults.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_EST_Init( __inout DRM_DST_CONTEXT *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_Uninit( __inout DRM_DST_CONTEXT *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CreateStore(
    __in_opt                           DRM_VOID          *f_pvOEMContext,
    __in_bcount( f_cbStoreData ) const DRM_BYTE          *f_pbStoreData,
    __in                               DRM_DWORD          f_cbStoreData,
    __in                         const DRM_DWORD          f_dwBlockSize,
    __in                               DRM_DST_BLKNUMTYPE f_eBlNumType,
    __in                               DRM_BOOL           f_fTruncateExist,
    __in                         const DRM_DWORD          f_dwInitSizeInKB,
    __out                              DRM_DST_CONTEXT   *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pvOEMContext   );
    UNREFERENCED_PARAMETER( f_pbStoreData    );
    UNREFERENCED_PARAMETER( f_cbStoreData    );
    UNREFERENCED_PARAMETER( f_dwBlockSize    );
    UNREFERENCED_PARAMETER( f_eBlNumType     );
    UNREFERENCED_PARAMETER( f_fTruncateExist );
    UNREFERENCED_PARAMETER( f_dwInitSizeInKB );
    UNREFERENCED_PARAMETER( f_pContextDST    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_OpenStore(
    __in_opt                     DRM_VOID        *f_pOEMContext,
    __in_bcount( f_cbStoreData ) DRM_BYTE        *f_pbStoreData,
    __in                         DRM_DWORD        f_cbStoreData,
    __in                         DRM_DWORD        f_dwGrowBySizeinKB,
    __out                        DRM_DST_CONTEXT *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pOEMContext      );
    UNREFERENCED_PARAMETER( f_pbStoreData      );
    UNREFERENCED_PARAMETER( f_cbStoreData      );
    UNREFERENCED_PARAMETER( f_dwGrowBySizeinKB );
    UNREFERENCED_PARAMETER( f_pContextDST      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CloseStore( __inout DRM_DST_CONTEXT *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CommitStore( __inout DRM_DST_CONTEXT *f_pContextDST )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CleanupStore(
    __inout DRM_DST_CONTEXT *f_pContextDST,
    __in    DRM_BOOL         f_fWait )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    UNREFERENCED_PARAMETER( f_fWait       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_OpenNamespace(
    __inout                            DRM_DST_CONTEXT           *f_pContextDST,
    __in    const                      DRM_DST_NAMESPACE         *f_pNamespace,
    __in                               DRM_DST_OPENMODE           f_eOpenMode,
    __in                               DRM_WORD                   f_wMaxNumChildren,
    __in                               DRM_DWORD                  f_dwExpectedSlotSize,
    __out                              DRM_DST_NAMESPACE_CONTEXT *f_pContextNS )
{
    UNREFERENCED_PARAMETER( f_pContextDST        );
    UNREFERENCED_PARAMETER( f_pNamespace         );
    UNREFERENCED_PARAMETER( f_eOpenMode          );
    UNREFERENCED_PARAMETER( f_wMaxNumChildren    );
    UNREFERENCED_PARAMETER( f_dwExpectedSlotSize );
    UNREFERENCED_PARAMETER( f_pContextNS         );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_DeleteNamespace(
    __inout    DRM_DST_CONTEXT   *f_pContextDST,
    __inout    DRM_DST_NAMESPACE *f_pNS,
    __in       DRM_BOOL           f_fWait )
{
    UNREFERENCED_PARAMETER( f_pContextDST );
    UNREFERENCED_PARAMETER( f_pNS         );
    UNREFERENCED_PARAMETER( f_fWait       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CloseNamespace(
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    UNREFERENCED_PARAMETER( f_pContextNS );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CommitNamespace( __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    UNREFERENCED_PARAMETER( f_pContextNS );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_MakeSlotHint(
    __in    DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out   DRM_DST_SLOT_HINT     *f_pSlotHint )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_pSlotHint    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_OpenSlot(
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in       DRM_DWORD                  f_dwMode,
    __in const DRM_DST_KEY               *f_pHashKey,
    __in const DRM_DST_KEY               *f_pUniqueKey,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __inout    DRM_DWORD                 *f_pcbSlotSize,
    __out      DRM_DST_SLOT_CONTEXT      *f_pContextSlot )
{
    UNREFERENCED_PARAMETER( f_pContextNS   );
    UNREFERENCED_PARAMETER( f_dwMode       );
    UNREFERENCED_PARAMETER( f_pHashKey     );
    UNREFERENCED_PARAMETER( f_pUniqueKey   );
    UNREFERENCED_PARAMETER( f_pSlotHint    );
    UNREFERENCED_PARAMETER( f_pcbSlotSize  );
    UNREFERENCED_PARAMETER( f_pContextSlot );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_CloseSlot( __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_DeleteSlot(
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in const DRM_DST_KEY               *f_pHashKey,
    __in const DRM_DST_KEY               *f_pUniqueKey,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __in       DRM_BOOL                   f_fWait )
{
    UNREFERENCED_PARAMETER( f_pContextNS );
    UNREFERENCED_PARAMETER( f_pHashKey   );
    UNREFERENCED_PARAMETER( f_pUniqueKey );
    UNREFERENCED_PARAMETER( f_pSlotHint  );
    UNREFERENCED_PARAMETER( f_fWait      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_InitSlotEnum(
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in_opt const DRM_DST_KEY               *f_pHashKey,
    __in           DRM_DST_LOCKMODE           f_eLockMode,
    __out          DRM_DST_ENUM_CONTEXT      *f_pContextEnum )
{
    UNREFERENCED_PARAMETER( f_pContextNS   );
    UNREFERENCED_PARAMETER( f_pHashKey     );
    UNREFERENCED_PARAMETER( f_eLockMode    );
    UNREFERENCED_PARAMETER( f_pContextEnum );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotEnumNext(
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pHashKey,
    __out_opt  DRM_DST_KEY           *f_pUniqueKey,
    __out_opt  DRM_DWORD             *f_pcbSlot )
{
    UNREFERENCED_PARAMETER( f_pContextEnum );
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_pHashKey     );
    UNREFERENCED_PARAMETER( f_pUniqueKey   );
    UNREFERENCED_PARAMETER( f_pcbSlot      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotEnumReloadCurrent(
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pHashKey,
    __out_opt  DRM_DST_KEY           *f_pUniqueKey,
    __out_opt  DRM_DWORD             *f_pcbSlot )
{
    UNREFERENCED_PARAMETER( f_pContextEnum );
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_pHashKey     );
    UNREFERENCED_PARAMETER( f_pUniqueKey   );
    UNREFERENCED_PARAMETER( f_pcbSlot      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotEnumDeleteCurrent(
    __inout   DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    UNREFERENCED_PARAMETER( f_pContextEnum );
    UNREFERENCED_PARAMETER( f_pContextSlot );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_BlockScanDelete(
    __inout                    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS,
    __in_ecount( f_cKeysHash ) DRM_DST_KEY                *f_rgKeyHash,
    __in                       DRM_DWORD                   f_cKeysHash,
    __in                       DRM_DST_KEY                *f_pkeyUnique,
    __in                       DRM_BOOL                    f_fWait )
{
    UNREFERENCED_PARAMETER( f_pContextNS );
    UNREFERENCED_PARAMETER( f_rgKeyHash  );
    UNREFERENCED_PARAMETER( f_cKeysHash  );
    UNREFERENCED_PARAMETER( f_pkeyUnique );
    UNREFERENCED_PARAMETER( f_fWait      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotResize(
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in     DRM_DWORD              f_cbSize )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_cbSize       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotSeek(
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in      DRM_LONG               f_lOffset,
    __in      DRM_DST_SEEKMODE       f_eOrigin,
    __out_opt DRM_DWORD             *f_pdwSeekPointer )
{
    UNREFERENCED_PARAMETER( f_pContextSlot   );
    UNREFERENCED_PARAMETER( f_lOffset        );
    UNREFERENCED_PARAMETER( f_eOrigin        );
    UNREFERENCED_PARAMETER( f_pdwSeekPointer );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotRead(
    __inout                  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                     DRM_DWORD              f_cbData,
    __out_bcount( f_cbData ) DRM_BYTE              *f_pbData,
    __out_opt                DRM_DWORD             *f_pcbRead )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_cbData       );
    UNREFERENCED_PARAMETER( f_pbData       );
    UNREFERENCED_PARAMETER( f_pcbRead      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotWrite(
    __inout                       DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                          DRM_DWORD              f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __out_opt                     DRM_DWORD             *f_pcbWritten )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_cbData       );
    UNREFERENCED_PARAMETER( f_pbData       );
    UNREFERENCED_PARAMETER( f_pcbWritten   );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_InitializeDST( __out DRM_DST *f_pDst )
{
    UNREFERENCED_PARAMETER( f_pDst );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotReadPtr(
    __inout                           DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __deref_out_bcount( *f_pcbData )  DRM_BYTE             **f_ppbData,
    __out                             DRM_DWORD             *f_pcbData )
{
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_ppbData      );
    UNREFERENCED_PARAMETER( f_pcbData      );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;
